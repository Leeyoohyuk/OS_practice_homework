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
	cout << "��ü �޸� ������ �Է� : ";
	cin >> memory_size;
	int *memory_pool = new int[memory_size];
	fill(memory_pool, memory_pool + memory_size, 0);

	while(1)
	{
		int pid, size;
		cout << "���μ��� Number (0 �Է½� ����) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "���μ��� Memory (0 �̸� Release) : ";
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
	} // ������ ��ġ Ž��
	for (int i = adr_temp; i < adr_temp + psize; i++)
	{
		temp[i] = pid;
	} // ������ �ش��ϴ� �����鿡 ���μ��� id ����

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
	} // ��� ������ �� ����

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
	} // �޸𸮸� �������ָ� �����ϴ� ���μ����� ������ġ, ����ġ�� ����Ѵ�.
	// start�� 0�̸� �� ó�� ���� release�� ��
	// end�� 0�̸� �� �� ���� release�� ��
	
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;
	int orgstart = start; // ���� ���μ��� �� ���۰� �� ���
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
	} // ��� ������ �� ����
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = " << total_usable / u_memory.size() << "K" << endl;
}