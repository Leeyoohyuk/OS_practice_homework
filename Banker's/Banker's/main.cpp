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
vector<process> user_proc;

int instance_define();
void proc_setting(int size);
void display_status();
void request();
bool safety();

int main()
{
	proc_setting(instance_define());
	display_status();
	request();
	return 0;
}

int instance_define()
{
	int instance_size, resource = 0;
	cout << "인스턴스 사이즈 입력 : ";
	cin >> instance_size;
	cout << "각 인스턴스 자원의 수 입력(in one line) : ";
	for (int i = 0; i < instance_size; i++)
	{
		cin >> resource;
		instance.push_back(resource);
	}
	return instance_size;
}

void proc_setting(int size)
{
	srand(time(NULL));
	int proc_size = 0;
	cout << "프로세스 개수 입력 : ";
	cin >> proc_size;
	int max, alloc, need;
	for (int i = 0; i < proc_size; i++)
	{
		process temp;
		int maxtemp; // 프로세스들에 할당된 resource가 최대 instance resource 를 넘지 않도록 함
		for (int j = 0; j < size; j++)
		{
			int max = rand() % (instance[j] - instance[j]/2 + 1);
			int alloc;
			do {
				alloc = rand() % (instance[j] / proc_size);
			} while (alloc > max);
			temp.max.push_back(max);
			temp.alloc.push_back(alloc);
			temp.need.push_back(max - alloc);
		}
		user_proc.push_back(temp);
	}
	//30 20 10 15 5
}

void request()
{
	cout << "Input Request instance " << instance.size();
	// 요청 돌리고 프로세스 돌아가면서 확인
	// 되는거 있으면 safety 검사
	// 되면 출력 안되면 안하고 다음 프로세스 검사
	// 끝까지 돌고없으면 안되는거
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
}