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

vector<usable> u_memory; // ��� ������ �޸� �ּҿ� ũ�� ����
vector<process> allocated; // ��� ���� �޸��� �ּҿ� ũ�� ����
void request(int msize, int pid, int psize);
void release(int msize, int pid);
void compaction(int msize);

int main()
{
	int memory_size;
	cout << "��ü �޸� ������ �Է� : ";
	cin >> memory_size;

	while (1)
	{
		int pid, size;
		cout << "�Ҵ� �� ���μ��� Number (0 �Է½� ����) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "�Ҵ� �� ���μ��� Memory (0 �̸� Release) : ";
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
	int min = msize; // ������ allocation ��ġ�� ã�� ���� ����
	int adr_temp = 0;  // �����ϴٰ� �����Ǵ� address
	int total_usable = 0; // ��� ������ �Ѱ��� compaction �۾� �Ǵܿ� ���
	for (int i = 0; i < u_memory.size(); i++)
	{
		total_usable += u_memory[i].size;
		if (u_memory[i].size >= psize && u_memory[i].size < min)
		{ // min���� �����ϸ� psize���ٴ� ũ�ų� ���� ��ġ�� ã�´�
			min = u_memory[i].size;
			adr_temp = u_memory[i].address;
		}
	}
	if (min == msize && total_usable > psize) // �޸� ������ ������ ������ ��ġ�� ������ compaction ����
	{
		compaction(msize); // sliding compaction ����
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
	} // ��� ������ ���� ����

	cout << "Best Fit: Allocated at address " << adr_temp << endl;
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0)
		cout << total_usable << "K" << endl;
	else
		cout << total_usable / u_memory.size() << "K" << endl;
}

void release(int msize, int pid)
{
	bool check = false; // Release �۾��� ����Ǵ��� üũ
	int psize = 0; // ���� ��û�� ���� ���μ����� �����ϴ� �޸� ũ��
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
		cout << "�������� �ʴ� Process�� ���� Release ��û�Դϴ�.\n";
		return;
	}
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;

	int orgstart = start; // ���� ���μ��� �� ���۰� �� ���
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
	} // ��� ������ �� ����
	cout << total_usable << "K free, " << u_memory.size() << " block(s), average size = ";
	if (total_usable == 0)
		cout << total_usable << "K" << endl;
	else
		cout << total_usable / u_memory.size() << "K" << endl;
}

void compaction(int msize) // compaction �۾�
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