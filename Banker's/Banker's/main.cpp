#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <iomanip>
using namespace std;

// process ����ü
struct process {
	vector<int> max; // �ִ� ��û ���� ���ҽ�
	vector<int> alloc; // ���� �������� ���ҽ�
	vector<int> need; // �ʿ��� ���ҽ� ( max - alloc )
};

vector<int> instance; // �ν��Ͻ��� ���� ��� ���ҽ�
vector<int> available; // �̿� ������ ���ҽ�
vector<process> user_proc; // ������ ������� ���μ���

int instance_define(); // �ν��Ͻ��� �Է¹޾� ����
bool proc_setting(int size); // ���μ��� ���� �Է¹޾� ��������
void display_status(); // ���� ���
void safety();

int main()
{
	if (proc_setting(instance_define()))
	{
		display_status();
		safety();
	}
	system("pause");
	return 0;
}

int instance_define()
{
	int instance_size, resource = 0;
	cout << "Input number of instance type : ";
	cin >> instance_size;
	cout << "�� �ν��Ͻ� �ڿ��� �� �� �� �Է�, �ڿ��� 0�� �� �����ϴ� : ";
	for (int i = 0; i < instance_size; i++)
	{
		cin >> resource;
		instance.push_back(resource);
		available.push_back(resource);
	} // �� �ν��Ͻ��� ���� ���ҽ��� �Է¹����� �̿� ������ �ڿ��� �����ϰ� �����
	// available�� ���� �� ���μ��� alloc ���� �����ϸ� ���ش�
	return instance_size;
}

bool proc_setting(int size)
{
	srand(time(NULL));
	int proc_size = 0;
	cout << "���μ��� ���� �Է� : ";
	cin >> proc_size;
	int max, alloc, need;
	for (int i = 0; i < proc_size; i++) // �Է¹��� ���μ��� �����ŭ �Ҵ�
	{
		process temp; // �ӽ� ����ü -> ���⿡ ���� �����ϰ� ���μ����� ����
		cout << "process" << i + 1 << " max �Ҵ� (instance �ڿ����� Ŭ �� �����ϴ�) -> ";
		for (int j = 0; j < size; j++)
		{
			cin >> max;
			temp.max.push_back(max);
			if (max > instance[j])
			{
				cout << "\n�߸��� max �����Դϴ� ���α׷��� �����մϴ�\n";
				return false;
			} // max���� �����ϰ� �׿� ���� �˻�
		}
		cout << "process" << i + 1 << " allocate �Ҵ� (available �ڿ����� Ŭ �� �����ϴ�) -> ";
		for (int j = 0; j < size; j++)
		{
			cin >> alloc;
			temp.alloc.push_back(alloc);
			if (available[j] < alloc || alloc > temp.max[j]) // �Ҵ��Ϸ��� �ڿ��� ��밡�� �ڿ����� ũ�ų� max���� Ŭ ���
			{
				cout << j << ' ' << available[j] << ' ' << alloc;
				cout << "\n�߸��� allocation �����Դϴ� ���α׷��� �����մϴ�\n";
				return false;
			} // �ڿ��� �Ҵ��ϰ� �̿� ������ �ڿ����� �Ҵ��� ��ŭ ���ش�
			available[j] -= alloc;
			temp.need.push_back(temp.max[j] - alloc);
		}
		user_proc.push_back(temp); // ��� �۾� �Ϸ� �� �� ���μ����� temp ����ü �Ҵ�
	}
	return true;
}

void safety()
{
	vector<int> proc_course; // ������ ���μ��� ���� ����
	int *Work = new int[instance.size()]; // ��� �����ϴٰ� ������ ���ҽ�
	for (int i = 0; i < instance.size(); i++)
	{
		Work[i] = available[i]; // �ʱ� ���� ���� available ���� �����´�
	}
	bool *Finish = new bool[user_proc.size()]; // �� ���μ����� �������� üũ�� �迭
	for (int i = 0; i < user_proc.size(); i++)
	{
		Finish[i] = false; // �ʱ� ���� ���� false�� �����Ѵ�
	}

	for (int i = 0; i < user_proc.size()*user_proc.size(); i++) 
	// �ִ� ���� Ƚ���� ���μ���^2�� ��������
	// i �� ����ؼ� �����ϱ� ������ process ������ % ����
	{
		bool check = true; // ���� �˻縦 ���� check ��
		if (!Finish[i % user_proc.size()]) // ���� ���� ���μ����� ��� ���� �˻�
		{
			for (int j = 0; j < instance.size(); j++)
			{
				if (user_proc[i % user_proc.size()].need[j] > Work[j])
					check = false; // ���μ��� Need�� �������� �ʰ��ϸ� check�� false��
			}
		}
		if (check && !Finish[i % user_proc.size()]) { // ���μ����� ���� �����̸� Need�� ������ �ʰ� �� �� ��� -> ��� ���� ����
			for (int j = 0; j < instance.size(); j++)
			{
				Work[j] += user_proc[i % user_proc.size()].alloc[j]; // Work�� ���μ����κ��� ��ȯ�Ǵ� allocated ���ҽ� �߰�
			}
			Finish[i % user_proc.size()] = true; // finish ǥ��
			proc_course.push_back(i % user_proc.size() +1); // ���μ��� ���� ��Ͽ� �߰�
		}
		if (proc_course.size() == user_proc.size()) // ���� ��ΰ� ������ ��� ���� ��� �� �Լ�����
		{
			cout << "\nThe safety processing course is \n";
			for (int j = 0; j < proc_course.size(); j++)
			{
				cout << "process" << proc_course[j] << " ";
			}
			cout << endl;
			return;
		}
	}
	cout << "\nThere isn't safety resource consuming root";
	return;
}

void display_status() // �ܼ� ��� �뵵
// �� instance�� �� resource ǥ��
// �� ���μ����� instance allocated, need, max ǥ��
// ���� available�� instance �ڿ� ǥ��
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