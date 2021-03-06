#include <iomanip>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <iostream>
using namespace std;

// 버퍼 구조체 정의
struct buffer
{
	int buffer_id; // 버퍼 아이디
	bool lock; // lock 상태 체크 true: lock, false: unlock
	bool write; // write 상태 체크
	bool delay; // delay 상태 체크
};

const int Hash_header_size = 5; // 해쉬 큐 헤더 사이즈는 5로 고정
int block;
vector<buffer> *hash_queue = new vector<buffer>[Hash_header_size];
vector<buffer> free_list; // 프리 리스트

// 해시 큐를 생성하는 함수
void create_Hash_Queue()
{
	int buffer_pool = 0; // 버퍼 풀 사이즈 변수
	srand(time(NULL));
	for (int i = 0; i < 5; i++)
	{
		int	table_size = rand() % 8 + 3; // 3~10 사이의 숫자로 각 헤쉬헤더가 갖는 block 수 지정
		buffer_pool += table_size; // 총 블럭 개수에 합산해준다
		while (table_size) // 헤더 사이즈에 맞는 할당이 끝날때까지 반복
		{
			bool redundancy_check = false;
			int buffer_num = rand() % 99 + 1;
			if (buffer_num % 5 != i) // 헤더 값 일치 검사
			{
				continue;
			}
			for (int j = 0; j < hash_queue[i].size(); j++) // 중복 값 탐색
			{
				if (hash_queue[i][j].buffer_id == buffer_num)
				{
					redundancy_check = true;
				}
			}
			if (!redundancy_check) { // 중복값이 없으면 추가
				buffer B;
				B.buffer_id = buffer_num;
				int attribute = rand() % 100; // 확률에 따라 각 상태를 설정한다
				if (attribute >= 82)
				{
					B.delay = false;
					B.lock = true;
					B.write = false;
				}
				else if (attribute >= 73)
				{
					B.delay = true; // 딜레이가 true 상태일 경우 무조건 free list 추가
					B.lock = false;
					B.write = false;
					free_list.push_back(B);
				}
				else { // unlock인 경우 35%확률로 free list에 추가
					B.delay = false;
					B.lock = false;
					B.write = false;
					attribute = rand() % 100;
					if (attribute >= 85)
						free_list.push_back(B);
				}
				hash_queue[i].push_back(B);
				table_size--;
			}
		}
	}
}

buffer getblk(int blk_num)
{
	while (true)
	{
		buffer *temp = nullptr; // 큐에 있는 버퍼의 상태를 조작하기 위해 포인터 사용
		bool block_in_hash = false;
		int hash_header = blk_num % 5;
		for (int i = 0; i < hash_queue[hash_header].size(); i++)
		{
			if (hash_queue[hash_header][i].buffer_id == blk_num) // 해시 큐
			{
				temp = &hash_queue[hash_header][i];
				block_in_hash = true;
				break;
			}
		}
		if (block_in_hash)
		{
			if (temp->lock) // 시나리오 5번 : 해시큐에서 블록 발견했지만 사용중 (=busy, lock) 인 경우
			{
				cout << "SC05 : The current process changes to a sleep state.\n\n";
				Sleep(3000);
				temp->lock = false;
				continue; // back to while loop
			}
			temp->lock = true; // 시나리오 1번 : 사용중이 아니라면 free_list에 있을 경우 free_list에서 끊어낸다
			for (int i = 0; i < free_list.size(); i++)
			{
				if (free_list[i].buffer_id == blk_num)
				{
					if (temp->delay)
					{
						temp->delay = false;
					}
					free_list.erase(free_list.begin() + i);
					break;
				}
			}
			return *temp;
		}
		else
		{
			buffer temp2;
			if (free_list.size() == 0)
			{
				cout << "SC04 : Free list is empty. The current process changes to a sleep state.\n\n";
				Sleep(3000); // 시나리오 4번 : 해시 큐에 존재하지 않으며 프리리스트가 비어있는 경우
				int randnum = rand() % 100;
				for (int i = 0; i < 5; i++) // + 임의 시나리오 - 다른 프로세스가 반납했다고 가정 : 첫번째 UNLOCK 발견 시 freelist에 추가
				{
					bool check = false;
					for (int j = 0; j < hash_queue[i].size(); j++)
					{
						if (hash_queue[i][j].lock == false && hash_queue[i][j].delay == false && hash_queue[i][j].write == false)
						{
							free_list.push_back(hash_queue[i][j]);
							check = true;
							break;
						}
					}
					if (check)
						break;
				}
				continue;
			}
			temp2 = free_list[0];
			if (temp2.delay) // 시나리오 3번 : 해시 큐에 존재하지 않으며 프리리스트상의 버퍼가 delay인 경우
			{
				for (int i = 0; i < hash_queue[temp2.buffer_id % 5].size(); i++)
				{
					if (hash_queue[temp2.buffer_id % 5][i].buffer_id == temp2.buffer_id)
					{
						hash_queue[temp2.buffer_id % 5][i].delay = false;
						hash_queue[temp2.buffer_id % 5][i].write = true;
						free_list.erase(free_list.begin()); // 프리리스트의 첫번째 값 삭제
					}
				}
				continue;
			}
			// 시나리오 2번 : free buffer를 찾아서 해시 큐에 할당
			int head_num = temp2.buffer_id % 5;
			free_list.erase(free_list.begin()); // 프리리스트의 첫번째 값 삭제
			for (int i = 0; i < hash_queue[head_num].size(); i++) // 해시 큐에서 값을 삭제한다
			{
				if (temp2.buffer_id == hash_queue[head_num][i].buffer_id)
				{
					hash_queue[head_num].erase(hash_queue[head_num].begin() + i);
					break;
				}
			}
			// 버퍼 재정의
			temp2.buffer_id = blk_num;
			temp2.delay = false;
			temp2.lock = true;
			temp2.write = false;
			hash_queue[blk_num % 5].push_back(temp2); // 갱신
			return temp2;
		}
		//delete temp;
	}
}

// 현재 버퍼 상태를 보여주는 함수
void display_Buffer()
{
	for (int i = 0; i < 5; i++)
	{
		cout << "[" << i << "]  ";
		cout.setf(ios::right);
		for (int j = 0; j < hash_queue[i].size(); j++)
		{
			cout << setw(6) << hash_queue[i][j].buffer_id;
		}
		cout << endl;
		cout << "STA  ";
		for (int j = 0; j < hash_queue[i].size(); j++)
		{
			if (hash_queue[i][j].delay)
			{
				cout << setw(6) << "DELAY";
			}
			else if (hash_queue[i][j].write)
			{
				cout << setw(6) << "WRITE";
			}
			else if (hash_queue[i][j].lock)
			{
				cout << setw(6) << "LOCK";
			}
			else
			{
				cout << setw(6) << "UNLCK";
			}
		}
		cout << endl;
	}
	cout << endl;
	cout << "----Free List-----\n";
	for (int i = 0; i < free_list.size(); i++)
	{
		cout << setw(5) << free_list[i].buffer_id;
	}
	cout << endl;
}

int main()
{
	create_Hash_Queue();

	display_Buffer();
	while (true)
	{
		cout << "\n\t 100을 입력하면 종료합니다 ----> ";
		cin >> block;
		if (block == 100)
			break;
		cout << endl;
		getblk(block);
		display_Buffer();
	}
}