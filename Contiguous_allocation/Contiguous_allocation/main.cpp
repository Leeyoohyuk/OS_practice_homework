#include <iostream>
#include <utility>
#include <vector>
#include <string>
using namespace std;

struct usable { // ��� ������ �޸� ������ ���� ����ü
	int address;
	int size;
};

struct process { // ���μ��� ������ ���� ����ü
	int pid;
	int address;
	int size;
};

vector<usable> u_memory; // ��� ������ �޸� �ּҿ� ũ�� ����
vector<process> allocated; // ���μ����� ID, �޸� �ּҿ� ũ�� ����
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
		cout << "�Ҵ� �� ���μ��� ID Number (0 �Է½� ����) : ";
		cin >> pid;
		if (pid == 0)
		{
			break;
		}
		cout << "�Ҵ� �� ���μ��� Memory size (0 �̸� Release) : ";
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
			cout << "�̹� �޸𸮿� �����ϴ� ���μ����Դϴ�\n";
			return;
		}
	}
	cout << "REQUEST " << pid << ": " << psize << "K" << endl;
	int min = msize; // psize�� ������ ��ġ�� ã�� ���� ���ŵǴ� ������ �Ǵ� ����
	int adr_temp = 0;  // ����� �޸� �ּҸ� ����ϴ� ����
	int total_usable = 0; // ��� ������ �Ѱ��� (compaction �۾� �Ǵܿ� ���)
	
	for (int i = 0; i < u_memory.size(); i++) // ��밡���� �޸� ������ scan
	{
		total_usable += u_memory[i].size;
		if (u_memory[i].size >= psize && u_memory[i].size < min)
		{ // min���� �����ϸ� psize�� ���ų� ū ������ ã�´� (���� ���� ��ġ ã�� ����)
			min = u_memory[i].size;
			adr_temp = u_memory[i].address;
		}
	}

	if (min == msize && total_usable >= psize) // ����� �޸� ������ ������ ���μ��� �Ҵ��� �Ұ����ϸ� compaction ����
	{
		compaction(msize); // sliding compaction
		adr_temp = u_memory[0].address;
	}
	if (msize < psize || (u_memory.size() != 0 && total_usable < psize)) // �Ұ����� ������ ��� error & return
	{
		cout << "REQUEST IMPOSSIBLE !!\n";
		return;
	}

	// allocated ���Ϳ� ���� �Ҵ�� ���μ����� �ִ´�
	process ptemp;
	ptemp.address = adr_temp;
	ptemp.pid = pid;
	ptemp.size = psize;
	vector<process>::iterator it;
	it = allocated.begin();
	if (allocated.size() == 0) // ������ �Ҵ�� ���μ����� ������� push_back
		allocated.push_back(ptemp);
	for (int i = 0; i < allocated.size(); i++) // �߰��� �� ������ �־���� ��� ��ġ ã�Ƽ� insert
	{
		if (allocated[i].address + allocated[i].size == ptemp.address)
			allocated.insert(it + i + 1, ptemp);
	}

	// ��� ������ ���� ����
	u_memory.clear();
	int before_end = 0; // ���� ���μ����� ������ ��ġ ���
	total_usable = 0; // �� ��� ������ ����
	for (int i = 0; i < allocated.size(); i++)
	{
		usable utemp;
		utemp.address = before_end; // ���� ���μ��� ���� �޸� �ּҸ� usable address�� �Ѵ�
		utemp.size = allocated[i].address - utemp.address; // ���� ���μ����� �ּҿ� ���Ͽ� ũ�� usable size ����
		before_end = allocated[i].address + allocated[i].size; // befor_end ����
		if (utemp.size != 0) // usable size�� 0�� �ƴ϶�� u_memory�� ��� �� total ����
		{
			u_memory.push_back(utemp);
			total_usable += utemp.size;
		}
		if (i + 1 == allocated.size()) // ���� ���μ����� �������̶�� usable�� �ڿ� �ֳ� �� �� �� scan �� �߰�
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
	bool check = false; // Release �۾��� ����Ǵ��� üũ
	int psize = 0; // ���� ��û�� ���� ���μ����� �����ϴ� �޸� ũ��
	int start = 0, end = 0;

	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].pid == pid) // ��û���� ���μ����� ��ġ�ϸ�
		{
			start = allocated[i].address;
			psize = allocated[i].size; 
			end = start + psize - 1;// ���μ����� �����ϴ� �޸� ����, �� ��ġ �� ũ�� ���
			allocated.erase(allocated.begin() + i); // ���μ��� ����
			check = true;
			break;
		}
	}
	if (!check) // ���μ��� �߰� ������ ��� error & return
	{
		cout << "�������� �ʴ� Process�� ���� Release ��û�Դϴ�.\n";
		return;
	}
	cout << "FREE REQUEST " << pid << ": " << psize << "K" << endl;
	cout << "Best Fit: Freed at address " << start << endl;

	int orgstart = start; // ������ ���μ��� �����ּ� ���
	if (start != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address + u_memory[i].size == start) // ������ ���� ������ usable�� ��� coalescing �۾�
			{
				start = u_memory[i].address; // coalescing ���� �޸� ���� �ּ� ���
				cout << "Coalescing blocks at addresses " << start << "K and " << orgstart << "K" << endl;
			}
		}
	}
	if (end != 0)
	{
		for (int i = 0; i < u_memory.size(); i++)
		{
			if (u_memory[i].address == end + 1) // ������ ���� ������ usable�� ��� coalescing �۾�
			{
				cout << "Coalescing blocks at addresses " << start << "K and " << end + 1 << "K" << endl;
			}
		}
	}

	// ���� usable scan �� ����� request�� ����
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

void compaction(int msize) // compaction �۾� (sliding ���)
{
	cout << "Compaction Start\n";
	int before_end = 0; // ���� ���μ����� �� ���
	for (int i = 0; i < allocated.size(); i++)
	{
		if (allocated[i].address != before_end) // ���� allocated process �ּҿ� �ٸ���� ���̿� usable�� ���� �ִٰ� �Ǵ��Ѵ�
		{
			cout << "Block at address " << allocated[i].address << " allocated at " << before_end << endl; // ���μ��� �޸� ��ġ ����
			allocated[i].address = before_end;
		}
		before_end = allocated[i].address + allocated[i].size; // before_end ����
	}

	// compaction ���Ŀ��� usable ������ ������ �ϳ��� �����
	u_memory.clear();
	usable utemp;
	utemp.address = before_end;
	utemp.size = msize - utemp.address;
	u_memory.push_back(utemp);
}