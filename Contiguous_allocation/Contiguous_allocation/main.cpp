#include <iostream>
#include <utility>
#include <vector>
#include <string>
using namespace std;

struct usable {
	int address;
	int size;
};

struct process {
	int pid;
	int address;
	int size;
};

vector<usable> u_memory; // 사용 가능한 메모리 주소와 크기 정보
vector<process> allocated; // 사용 중인 메모리의 주소와 크기 정보
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
		cout << "할당 할 프로세스 Number (0 입력시 종료) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "할당 할 프로세스 Memory (0 이면 Release) : ";
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
	cout << "REQUEST " << pid << ": " << psize << "K" << endl;
	int min = msize; // 적절한 allocation 위치를 찾기 위한 변수
	int adr_temp = 0;  // 적합하다고 생각되는 address
	int total_usable = 0; // 사용 가능한 총공간 compaction 작업 판단에 사용
	for (int i = 0; i < u_memory.size(); i++)
	{
		total_usable += u_memory[i].size;
		if (u_memory[i].size >= psize && u_memory[i].size < min)
		{ // min값을 갱신하며 psize보다는 크거나 같은 위치를 찾는다
			min = u_memory[i].size;
			adr_temp = u_memory[i].address;
		}
	}
	if (min == msize && total_usable > psize) // 메모리 공간은 있으나 적합한 위치가 없으면 compaction 시행
	{
		compaction(msize); // sliding compaction 시행
		adr_temp = u_memory[0].address;
	}
	if (msize < psize || (u_memory.size() != 0 && total_usable < psize))
	{
		cout << "REQUEST IMPOSSIBLE !!";
		return;
	}

	process ptemp;
	ptemp.address = adr_temp;
	ptemp.pid = pid;
	ptemp.size = psize;
	vector<process>::iterator it;
	it = allocated.begin();
	if (allocated.size() == 0)
		allocated.push_back(ptemp);
	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].address + allocated[i].size == ptemp.address)
			allocated.insert(it + i + 1, ptemp);
	}

	u_memory.clear();
	int before_end = 0;
	total_usable = 0;
	for (int i = 0; i < allocated.size(); i++)
	{
		usable utemp;
		utemp.address = before_end;
		utemp.size = allocated[i].address - utemp.address;
		before_end = allocated[i].address + allocated[i].size; // 64
		if (utemp.size != 0)
		{
			u_memory.push_back(utemp);
			total_usable += utemp.size;
		}
		if (i + 1 == allocated.size())
		{
			usable utemp;
			utemp.address = before_end;
			utemp.size = msize - utemp.address;
			if (utemp.size != 0)
			{
				u_memory.push_back(utemp);
				total_usable += utemp.size;
			}
		}
	} // 사용 가능한 공간 조사

	cout << "Best Fit: Allocated at address " << adr_temp << endl;
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0)
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
		if (allocated[i].pid == pid)
		{
			start = allocated[i].address;
			psize = allocated[i].size;
			end = start + psize - 1;
			allocated.erase(allocated.begin() + i);
			check = true;
			break;
		}
	}
	if (!check)
	{
		cout << "존재하지 않는 Process에 대한 Release 요청입니다.\n";
		return;
	}
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;

	int orgstart = start; // 이전 프로세스 블럭 시작과 끝 기억
	if (start != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address + u_memory[i].size == start)
			{
				start = u_memory[i].address;
				cout << "Coalescing blocks at addresses " << start << "K and " << orgstart << "K" << endl;
			}
		}
	}
	if (end != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address == end + 1)
			{
				cout << "Coalescing blocks at addresses " << start << "K and " << end + 1 << "K" << endl;
			}
		}
	}

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
			usable utemp;
			utemp.address = before_end;
			utemp.size = msize - utemp.address;
			if (utemp.size != 0)
			{
				u_memory.push_back(utemp);
				total_usable += utemp.size;
			}
		}
	} // 사용 가능한 블럭 조사
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0)
		cout << total_usable << "K" << endl;
	else
		cout << total_usable / u_memory.size() << "K" << endl;
}

void compaction(int msize) // compaction 작업
{
	cout << "Compaction Start\n";
	int before_end = 0;
	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].address != before_end)
		{
			cout << "Block at address " << allocated[i].address << " allocated at " << before_end << endl;
			allocated[i].address = before_end;
		}
		before_end = allocated[i].address + allocated[i].size;
	}
	u_memory.clear();
	usable utemp;
	utemp.address = before_end;
	utemp.size = msize - utemp.address;
	u_memory.push_back(utemp);
}