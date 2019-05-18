#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <iomanip>
using namespace std;

// process 구조체
struct process {
	vector<int> max; // 최대 요청 가능 리소스
	vector<int> alloc; // 현재 점유중인 리소스
	vector<int> need; // 필요한 리소스 ( max - alloc )
};

vector<int> instance; // 인스턴스가 가진 모든 리소스
vector<int> available; // 이용 가능한 리소스
vector<process> user_proc; // 유저가 사용중인 프로세스

int instance_define(); // 인스턴스를 입력받아 정의
bool proc_setting(int size); // 프로세스 개수 입력받아 랜덤세팅
void display_status(); // 상태 출력
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
	cout << "각 인스턴스 자원의 수 한 줄 입력, 자원은 0일 수 없습니다 : ";
	for (int i = 0; i < instance_size; i++)
	{
		cin >> resource;
		instance.push_back(resource);
		available.push_back(resource);
	} // 각 인스턴스가 갖는 리소스를 입력받으며 이용 가능한 자원도 동일하게 만든다
	// available은 이후 각 프로세스 alloc 값을 세팅하며 빼준다
	return instance_size;
}

bool proc_setting(int size)
{
	srand(time(NULL));
	int proc_size = 0;
	cout << "프로세스 개수 입력 : ";
	cin >> proc_size;
	int max, alloc, need;
	for (int i = 0; i < proc_size; i++) // 입력받은 프로세스 사이즈만큼 할당
	{
		process temp; // 임시 구조체 -> 여기에 값을 세팅하고 프로세스에 저장
		cout << "process" << i + 1 << " max 할당 (instance 자원보다 클 수 없습니다) -> ";
		for (int j = 0; j < size; j++)
		{
			cin >> max;
			temp.max.push_back(max);
			if (max > instance[j])
			{
				cout << "\n잘못된 max 설정입니다 프로그램을 종료합니다\n";
				return false;
			} // max값을 설정하고 그에 대해 검사
		}
		cout << "process" << i + 1 << " allocate 할당 (available 자원보다 클 수 없습니다) -> ";
		for (int j = 0; j < size; j++)
		{
			cin >> alloc;
			temp.alloc.push_back(alloc);
			if (available[j] < alloc || alloc > temp.max[j]) // 할당하려는 자원이 사용가능 자원보다 크거나 max보다 클 경우
			{
				cout << j << ' ' << available[j] << ' ' << alloc;
				cout << "\n잘못된 allocation 설정입니다 프로그램을 종료합니다\n";
				return false;
			} // 자원을 할당하고 이용 가능한 자원에서 할당한 만큼 빼준다
			available[j] -= alloc;
			temp.need.push_back(temp.max[j] - alloc);
		}
		user_proc.push_back(temp); // 모든 작업 완료 후 한 프로세스에 temp 구조체 할당
	}
	return true;
}

void safety()
{
	vector<int> proc_course; // 안전한 프로세스 수행 순서
	int *Work = new int[instance.size()]; // 사용 가능하다고 가정된 리소스
	for (int i = 0; i < instance.size(); i++)
	{
		Work[i] = available[i]; // 초기 값은 현재 available 값을 가져온다
	}
	bool *Finish = new bool[user_proc.size()]; // 각 프로세스가 끝났는지 체크할 배열
	for (int i = 0; i < user_proc.size(); i++)
	{
		Finish[i] = false; // 초기 값은 전부 false로 세팅한다
	}

	for (int i = 0; i < user_proc.size()*user_proc.size(); i++) 
	// 최대 수행 횟수는 프로세스^2를 넘지않음
	// i 는 계속해서 증가하기 때문에 process 갯수로 % 연산
	{
		bool check = true; // 조건 검사를 위한 check 값
		if (!Finish[i % user_proc.size()]) // 수행 이전 프로세스인 경우 조건 검사
		{
			for (int j = 0; j < instance.size(); j++)
			{
				if (user_proc[i % user_proc.size()].need[j] > Work[j])
					check = false; // 프로세스 Need가 보유분을 초과하면 check는 false로
			}
		}
		if (check && !Finish[i % user_proc.size()]) { // 프로세스가 수행 이전이며 Need가 보유분 초과 안 할 경우 -> 모든 조건 만족
			for (int j = 0; j < instance.size(); j++)
			{
				Work[j] += user_proc[i % user_proc.size()].alloc[j]; // Work에 프로세스로부터 반환되는 allocated 리소스 추가
			}
			Finish[i % user_proc.size()] = true; // finish 표시
			proc_course.push_back(i % user_proc.size() +1); // 프로세스 수행 목록에 추가
		}
		if (proc_course.size() == user_proc.size()) // 가능 경로가 정해진 경우 순서 출력 후 함수종료
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

void display_status() // 단순 출력 용도
// 각 instance의 총 resource 표시
// 각 프로세스의 instance allocated, need, max 표시
// 현재 available한 instance 자원 표시
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