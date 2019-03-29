#include "pch.h"
#include <stdio.h>
#include <vector>
#include <ctime>
#include <Windows.h>
#include <iostream>
using namespace std;

const int Hash_header_size = 5; // 해쉬 큐 헤더 사이즈는 5로 고정
int block;
vector<buffer> *hash_queue = new vector<buffer>[Hash_header_size];
vector<buffer> free_list; // 프리 리스트

// 버퍼 구조체 정의
struct buffer
{
	int buffer_id; // 버퍼 아이디
	bool lock; // lock 상태 체크 true: lock, false: unlock
	bool write; // write 상태 체크
	bool delay; // delay 상태 체크
};

// 해시 큐를 생성하는 함수
void create_Hash_Queue()
{
	int buffer_pool = 0; // 버퍼 풀 사이즈 변수
	srand(time(NULL));
	int table_size; // 해시 큐 헤더 당 테이블의 사이즈
	for (int i = 0; i < 5; i++)
	{
		table_size = rand() % 8 + 3; // 3~10 사이의 숫자로 각 헤쉬테이블 사이즈를 잡는다
		int cnt = table_size; // 반복문에 사용될 카운트 할당
		buffer_pool += table_size;
		while (cnt)
		{
			int buffer_num = rand() % 100;
			if (buffer_num % 5 == i) {
				buffer B;
				B.buffer_id = buffer_num;
				int attribute = rand() % 100; // 속성 확률에 따라 각 속성을 설정한다
				if (attribute >= 80)
				{
					B.delay = false;
					B.lock = true;
					B.write = false;
				}
				else if (attribute >= 70)
				{
					B.delay = true; // 딜레이가 true인 속성일 경우 무조건 free list 추가
					B.lock = false;
					B.write = false;
					free_list.push_back(B);
				}
				else { // unlock인 경우 35%확률로 free list에 추가
					B.delay = false;
					B.lock = false;
					B.write = false;
					attribute = rand() % 100;
					if (attribute >= 65)
						free_list.push_back(B);
				}
				hash_queue[i].push_back(B);
				cnt--;
			}
		}
	}
}

buffer getblk(int blk_num)
{
	bool buffer_check = false;
	buffer *temp;
	while (!buffer_check)
	{
		bool block_in_hash = false;
		int hash_header = blk_num % 5;
		for (int i = 0; i < hash_queue[hash_header].size(); i++)
		{
			if (hash_queue[hash_header][i] == blk_num) // 해시 큐
			{
				temp = &hash_queue[hash_header][i];
				buffer_check = true;
				break;
			}
		}
		if (block_in_hash)
		{
			if (temp->lock) // 시나리오 5번 : 해시큐에서 블록 발견했지만 사용중 (=busy, lock) 인 경우
			{
				Sleep(3000);
				temp->lock = false;
				continue; // back to while loop
			}
			temp->lock = true; // 시나리오 1번 : 사용중이 아니라면 free_list에 있을 경우 free_list에서 끊어낸다
			for (int i = 0; i < free_list.size(); i++)
			{
				if (free_list[i].buffer_id == blk_num)
				{
					free_list.erase(free_list.begin() + i);
					break;
				}
			}
			return *temp;
		}
		else
		{
			buffer temp2; // 해시 큐에 해당 버퍼가 없는 경우이기 때문에 값 조작필요X, 따라서 포인터가 아닌 일반 변수선언
			if (free_list.size() == 0)
			{
				Sleep(3000);
				int randnum = rand() % 100;
				// 구현 필요---------------------------------------
				continue;
			}
			temp2 = free_list[0];
			free_list.erase(free_list.begin()); // 프리리스트의 첫번째 값 삭제
			if (temp->delay) // 시나리오 4번 : 해시 큐에 존재하지 않으며 프리리스트상의 버퍼가 delay인 경우
			{
				temp2.delay = false;
				temp2.write = true;
				continue;
			}
			// 시나리오 2번 : free buffer를 찾아서 해시 큐에 할당
			int head_num = temp2.buffer_id % 5;
			for (int i = 0; i < hash_queue[head_num].size(); i++) // 해시 큐에서 값을 삭제한다
			{
				if (temp2.buffer_id == hash_queue[head_num][i])
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
	}
}

void display_Buffer()
{
	// 구현 필요---------------------------------------
}

int main()
{
	create_Hash_Queue();
	cin >> block;
	getblk(block);
}