#include <iostream>
#include <utility>
#include <vector>
#include <string>
using namespace std;

struct usable { // 사용 가능한 메모리 정보에 대한 구조체
	int address;
	int size;
};

struct process { // 프로세스 정보에 대한 구조체
	int pid;
	int address;
	int size;
};

vector<usable> u_memory; // 사용 가능한 메모리 주소와 크기 정보
vector<process> allocated; // 프로세스의 ID, 메모리 주소와 크기 정보
void request(int msize, int pid, int psize);
void release(int msize, int pid);
void compaction(int msize);

int main()
{
	int memory_size;
	cout << "전체 메모리 사이즈 입력 : ";
	cin >> memory_size;

	while (1)
	{
		int pid, size;
		cout << "할당 할 프로세스 ID Number (0 입력시 종료) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "할당 할 프로세스 Memory size (0 이면 Release) : ";
		cin >> size;
		if (size != 0)
			request(memory_size, pid, size);
		else
			release(memory_size, pid);
		cout << endl;
	}
	return 0;
}

void request(int msize, int pid, int psize)
{
	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].pid == pid)
		{
			cout << "이미 메모리에 존재하는 프로세스입니다\n";
			return;
		}
	}
	cout << "REQUEST " << pid << ": " << psize << "K" << endl;
	int min = msize; // psize에 적절한 위치를 찾기 위해 갱신되는 기준이 되는 변수
	int adr_temp = 0;  // 저장된 메모리 주소를 기록하는 변수
	int total_usable = 0; // 사용 가능한 총공간 (compaction 작업 판단에 사용)
	
	for (int i = 0; i < u_memory.size(); i++) // 사용가능한 메모리 공간을 scan
	{
		total_usable += u_memory[i].size;
		if (u_memory[i].size >= psize && u_memory[i].size < min)
		{ // min값을 갱신하며 psize와 같거나 큰 공간을 찾는다 (가장 작은 위치 찾기 위함)
			min = u_memory[i].size;
			adr_temp = u_memory[i].address;
		}
	}

	if (min == msize && total_usable >= psize) // 충분한 메모리 공간이 있으나 프로세스 할당이 불가능하면 compaction 시행
	{
		compaction(msize); // sliding compaction
		adr_temp = u_memory[0].address;
	}
	if (msize < psize || (u_memory.size() != 0 && total_usable < psize)) // 불가능한 조건인 경우 error & return
	{
		cout << "REQUEST IMPOSSIBLE !!\n";
		return;
	}

	// allocated 벡터에 새로 할당된 프로세스를 넣는다
	process ptemp;
	ptemp.address = adr_temp;
	ptemp.pid = pid;
	ptemp.size = psize;
	vector<process>::iterator it;
	it = allocated.begin();
	if (allocated.size() == 0) // 이전에 할당된 프로세스가 없을경우 push_back
		allocated.push_back(ptemp);
	for (int i = 0; i < allocated.size(); i++) // 중간의 빈 공간에 넣어야할 경우 위치 찾아서 insert
	{
		if (allocated[i].address + allocated[i].size == ptemp.address)
			allocated.insert(it + i + 1, ptemp);
	}

	// 사용 가능한 공간 갱신
	u_memory.clear();
	int before_end = 0; // 이전 프로세스의 끝나는 위치 기록
	total_usable = 0; // 총 사용 가능한 공간
	for (int i = 0; i < allocated.size(); i++)
	{
		usable utemp;
		utemp.address = before_end; // 이전 프로세스 다음 메모리 주소를 usable address로 한다
		utemp.size = allocated[i].address - utemp.address; // 현재 프로세스의 주소와 비교하여 크기 usable size 측정
		before_end = allocated[i].address + allocated[i].size; // befor_end 갱신
		if (utemp.size != 0) // usable size가 0이 아니라면 u_memory에 기록 및 total 갱신
		{
			u_memory.push_back(utemp);
			total_usable += utemp.size;
		}
		if (i + 1 == allocated.size()) // 현재 프로세스가 마지막이라면 usable이 뒤에 있나 한 번 더 scan 및 추가
		{
			utemp.address = before_end;
			utemp.size = msize - utemp.address;
			if (utemp.size != 0)
			{
				u_memory.push_back(utemp);
				total_usable += utemp.size;
			}
		}
	}

	cout << "Best Fit: Allocated at address " << adr_temp << endl;
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0) // divide 0 error handling
		cout << total_usable << "K" << endl;
	else
		cout << total_usable / u_memory.size() << "K" << endl;
}

void release(int msize, int pid)
{
	bool check = false; // Release 작업이 진행되는지 체크
	int psize = 0; // 해제 요청을 받은 프로세스가 점유하는 메모리 크기
	int start = 0, end = 0;

	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].pid == pid) // 요청받은 프로세스와 일치하면
		{
			start = allocated[i].address;
			psize = allocated[i].size; 
			end = start + psize - 1;// 프로세스가 점유하던 메모리 시작, 끝 위치 및 크기 기록
			allocated.erase(allocated.begin() + i); // 프로세스 삭제
			check = true;
			break;
		}
	}
	if (!check) // 프로세스 발견 못햇을 경우 error & return
	{
		cout << "존재하지 않는 Process에 대한 Release 요청입니다.\n";
		return;
	}
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;

	int orgstart = start; // 삭제된 프로세스 시작주소 기억
	if (start != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address + u_memory[i].size == start) // 삭제된 공간 앞쪽이 usable할 경우 coalescing 작업
			{
				start = u_memory[i].address; // coalescing 이후 메모리 시작 주소 기록
				cout << "Coalescing blocks at addresses " << start << "K and " << orgstart << "K" << endl;
			}
		}
	}
	if (end != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address == end + 1) // 삭제된 공간 뒤쪽이 usable할 경우 coalescing 작업
			{
				cout << "Coalescing blocks at addresses " << start << "K and " << end + 1 << "K" << endl;
			}
		}
	}

	// 이하 usable scan 및 출력은 request와 동일
	u_memory.clear();
	int before_end = 0;
	int total_usable = 0;
	for (int i = 0; i < allocated.size(); i++)
	{
		usable utemp;
		utemp.address = before_end;
		utemp.size = allocated[i].address - utemp.address;
		before_end = allocated[i].address + allocated[i].size;
		if (utemp.size != 0)
		{
			u_memory.push_back(utemp);
			total_usable += utemp.size;
		}
		if (i + 1 == allocated.size())
		{
			utemp.address = before_end;
			utemp.size = msize - utemp.address;
			if (utemp.size != 0)
			{
				u_memory.push_back(utemp);
				total_usable += utemp.size;
			}
		}
	}

	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0)
		cout << total_usable << "K" << endl;
	else
		cout << total_usable / u_memory.size() << "K" << endl;
}

void compaction(int msize) // compaction 작업 (sliding 방식)
{
	cout << "Compaction Start\n";
	int before_end = 0; // 이전 프로세스의 끝 기록
	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].address != before_end) // 현재 allocated process 주소와 다를경우 사이에 usable한 공간 있다고 판단한다
		{
			cout << "Block at address " << allocated[i].address << " allocated at " << before_end << endl; // 프로세스 메모리 위치 변경
			allocated[i].address = before_end;
		}
		before_end = allocated[i].address + allocated[i].size; // before_end 갱신
	}

	// compaction 이후에는 usable 공간은 무조건 하나만 생긴다
	u_memory.clear();
	usable utemp;
	utemp.address = before_end;
	utemp.size = msize - utemp.address;
	u_memory.push_back(utemp);
}