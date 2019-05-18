#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <iomanip>
using namespace std;

struct process {
	vector<int> max;
	vector<int> alloc;
	vector<int> need;
};

vector<int> instance;
vector<int> available;
vector<process> user_proc;

int instance_define();
void proc_setting(int size);
void display_status();
void safety();

int main()
{
	proc_setting(instance_define());
	display_status();
	safety();
	return 0;
}

int instance_define()
{
	int instance_size, resource = 0;
	cout << "�ν��Ͻ� ������ �Է� : ";
	cin >> instance_size;
	cout << "�� �ν��Ͻ� �ڿ��� �� �Է�(in one line) : ";
	for (int i = 0; i < instance_size; i++)
	{
		cin >> resource;
		instance.push_back(resource);
		available.push_back(resource);
	}
	return instance_size;
}

void proc_setting(int size)
{
	srand(time(NULL));
	int proc_size = 0;
	cout << "���μ��� ���� �Է� : ";
	cin >> proc_size;
	int max, alloc, need;
	for (int i = 0; i < proc_size; i++)
	{
		process temp;
		int maxtemp; // ���μ����鿡 �Ҵ�� resource�� �ִ� instance resource �� ���� �ʵ��� ��
		for (int j = 0; j < size; j++)
		{
			int max = rand() % ((instance[j] / 4)*3 + 1);
			int alloc;
			do {
				alloc = rand() % ((instance[j] / proc_size) + 1);
			} while (alloc > max);
			temp.max.push_back(max);
			temp.alloc.push_back(alloc);
			temp.need.push_back(max - alloc);
			available[j] -= alloc;
		}
		user_proc.push_back(temp);
	}
}

void safety()
{
	vector<int> proc_course;
	int *Work = new int[instance.size()];
	for (int i = 0; i < instance.size(); i++)
	{
		Work[i] = available[i];
	}
	bool *Finish = new bool[user_proc.size()];
	for (int i = 0; i < user_proc.size(); i++)
	{
		Finish[i] = false;
	}
	cout << endl;
	for (int i = 0; i < user_proc.size(); i++)
	{
		bool check = true;
		if (!Finish[i])
		{
			for (int j = 0; j < instance.size(); j++)
			{
				if (user_proc[i].need[j] > Work[j])
					check = false; // ���μ��� Need�� �������� �ʰ��ϸ� �ش� ���μ����� ����X
			}
		}
		if (check && !Finish[i]) {
			for (int j = 0; j < instance.size(); j++)
			{
				Work[j] += user_proc[i].alloc[j];
			}
			Finish[i] = true;
			proc_course.push_back(i+1); // �ڽ� �߰� ���Ŀ� �ڽ���������Ͽ� ��� 
			i = -1; // �������� �����Ѱ� �ִ��� �ٽ� �˻�
		}
	}
	if (proc_course.size() == user_proc.size())
	{
		cout << "\nThe safety processing course is ";
		for (int i = 0; i < proc_course.size(); i++)
		{
			cout << "process" << proc_course[i] << " ";
		}
		cout << endl;
	}
	else
		cout << "\nThere isn't safety resource consuming root";
	return;
}

void display_status()
{
	cout << "\n Instance Resource status \n\n";
	for (int i = 0; i<instance.size(); i++)
	{
		cout << "ins " << i + 1 << "total resoucrce: " << instance[i] << ' ' << endl;
	}
	for (int i = 0; i < user_proc.size(); i++)
	{
		cout << "\n    Process " << i + 1 << " status";
		
		cout << "\n    Allocated resource \n";
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(7) << "ins" << j + 1;
		}
		cout << endl;
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(8) << user_proc[i].alloc[j];
		}

		cout << "\n         Need resource \n";
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(7) << "ins" << j + 1;
		}
		cout << endl;
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(8) << user_proc[i].need[j];
		}

		cout << "\n          Max resource \n";
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(7) << "ins" << j + 1;
		}
		cout << endl;
		for (int j = 0; j < instance.size(); j++)
		{
			cout << setw(8) << user_proc[i].max[j];
		}
		cout << endl;
	}
	cout << "\n    Available resource \n";
	for (int j = 0; j < instance.size(); j++)
	{
		cout << setw(7) << "ins" << j + 1;
	}
	cout << endl;
	for (int j = 0; j < instance.size(); j++)
	{
		cout << setw(8) << available[j];
	}
}