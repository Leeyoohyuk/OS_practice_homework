#include <iostream>
#include <utility>
#include <vector>
#include <string>
using namespace std;

struct usable {
	int address;
	int size;
};

vector<usable> u_memory;
void request(int temp[], int pid, int psize);
void release(int temp[], int pid);

int main()
{
	int memory_size;
	cout << "전체 메모리 사이즈 입력 : ";
	cin >> memory_size;
	int *memory_pool = new int[memory_size];
	fill(memory_pool, memory_pool + memory_size, 0);

	while(1)
	{
		int pid, size;
		cout << "프로세스 Number (0 입력시 종료) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "프로세스 Memory (0 이면 Release) : ";
		cin >> size;
		if (size != 0)
			request(memory_pool, pid, size);
		else
			release(memory_pool, pid);
		cout << endl;
	}
	return 0;
}

void request(int temp[], int pid , int psize)
{
	cout << "REQUEST " << pid << ": " << psize << "K" << endl;
	int size = _msize(temp) / 4;
	int min = size;
	int adr_temp = 0;
	for (int i = 0; i < u_memory.size(); i++)
	{
		if (u_memory[i].size >= psize && u_memory[i].size < min)
		{
			min = u_memory[i].size;
			adr_temp = u_memory[i].address;
		}
	} // 적합한 위치 탐색
	for (int i = adr_temp; i < adr_temp + psize; i++)
	{
		temp[i] = pid;
	} // 범위에 해당하는 공간들에 프로세스 id 대입

	int u_size = 0;
	int u_adr = 0;
	int total_usable = 0;
	u_memory.clear();
	for (int i = 0; i < size; i++)
	{
		if (i == size - 1 && u_size != 0)
		{
			total_usable++;
			u_size++;
			usable temp;
			temp.size = u_size;
			temp.address = u_adr;
			u_memory.push_back(temp);
		}
		else if (temp[i] == 0)
		{
			total_usable++;
			u_size++;
			if (u_size == 1)
				u_adr = i;
		}
		else if (temp[i] != 0 && u_size != 0)
		{
			usable temp;
			temp.size = u_size;
			temp.address = u_adr;
			u_memory.push_back(temp);
			u_size = 0;
		}
	} // 사용 가능한 블럭 조사

	cout << "Best Fit: Allocated at address " << adr_temp << endl;
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = " << total_usable/u_memory.size() << "K" << endl;
}

void release(int temp[], int pid)
{
	int size = _msize(temp) / 4;
	int psize = 0;
	int start = 0, end = 0;
	for (int i = 0; i < size; i++)
	{
		if (temp[i] == pid)
		{
			temp[i] = 0;
			psize++;
			if (psize == 1)
				start = i;
		}
		else if (temp[i] != pid && psize != 0)
		{
			end = i-1;
			break;
		}
	} // 메모리를 해제해주며 존재하던 프로세스의 시작위치, 끝위치를 기록한다.
	// start가 0이면 맨 처음 블럭을 release한 것
	// end가 0이면 맨 끝 블럭을 release한 것
	
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;
	int orgstart = start; // 이전 프로세스 블럭 시작과 끝 기억
	int orgend = end;

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
				end = u_memory[i].address + u_memory[i].size;
				cout << "Coalescing blocks at addresses " << orgend + 1 << "K and " << end << "K" << endl;
			}
		}
	}
	
	int u_size = 0;
	int u_adr = 0;
	int total_usable = 0;
	u_memory.clear();
	for (int i = 0; i < size; i++)
	{
		if (i == size - 1 && u_size != 0)
		{
			total_usable++;
			u_size++;
			usable temp;
			temp.size = u_size;
			temp.address = u_adr;
			u_memory.push_back(temp);
		}
		else if (temp[i] == 0)
		{
			total_usable++;
			u_size++;
			if (u_size == 1)
				u_adr = i;
		}
		else if (temp[i] != 0 && u_size != 0)
		{
			usable temp;
			temp.size = u_size;
			temp.address = u_adr;
			u_memory.push_back(temp);
			u_size = 0;
		}
	} // 사용 가능한 블럭 조사
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = " << total_usable / u_memory.size() << "K" << endl;
}