#include <iostream>
#include "IDEcontroller.h"
#include <thread>
#include <Windows.h>
#include <vector>
#include <string>
#include <random>
using namespace std;

int size();
int type();
void task_processing();

int main()
{
	task_processing();
	return 0;
}

int size()
{
	int tasksize = 0;
	cout << " Please enter the number of tasks you want to task : ";
	cin >> tasksize;
	while (tasksize < 500) // 500 미만의 type을 입력시 재입력 받도록 함
	{
		cout << " Please enter at least 500 tasks : ";
		cin >> tasksize;
	}
	return tasksize;
}

int type()
{
	int num_of_device = 0;
	cout << " Please enter the number of type you want to task (at least 3type) : ";
	cin >> num_of_device;
	while (num_of_device < 3) // 3 미만의 type을 입력시 재입력 받도록 함
	{
		cout << " Please enter a number of more than three : ";
		cin >> num_of_device;
	}
	return num_of_device;
}

void task_processing()
{
	random_device ranum;
	string device[5] = { "keyboard", "mouse", "monitor", "printer", "disk" }; // 테스킹 device type
	bool flag[5] = { 0 };
	int task_size = size(); // task의 개수
	int num_of_device = type(); // type의 개수

	IDEcontroller *idecon = new IDEcontroller[num_of_device]; // IDEcontroller 객체 동적할당
	thread *ide = new thread[num_of_device * 2]; // 사용할 devcie 개수 * 2 만큼의 스레드 할당 -> 할당과 처리의 분리
	queue<pair<string, int>> *DMAcontroller = new queue<pair<string, int>>;
	for (int i = 0; i < num_of_device; i++)
	{
		int randnum;
		do {
			randnum = ranum() % 5;
		} while (flag[randnum]);
		flag[randnum] = true;
		idecon[i].setdevice(device[randnum]);
	} // IDEcontroller에 랜덤으로 device 설정

	for (int i = 0; i < num_of_device; i++)
	{
		ide[i] = thread(&IDEcontroller::assign, &idecon[i], task_size);
		ide[i + num_of_device] = thread(&IDEcontroller::processing, &idecon[i], DMAcontroller, task_size);
	} // 멀티스레딩을 이용하여 assign - processing 작업 시작
	for (int i = 0; i < num_of_device * 2; i++)
	{
		ide[i].join();
	} // 모든 작업 종료 시 알림
	cout << " All task on IDEcontroller have been processed\n";
	cout << "\n Start assigning on the memory buffer\n\n";
	Sleep(5000); // 출력확인 용 sleep

	while (DMAcontroller->size())
	{
		cout << ' ' << DMAcontroller->front().first << 'X' << DMAcontroller->front().second << " have been transfered on the memory buffer\n";
		DMAcontroller->pop(); // 메모리 버퍼에 전달
	}
	cout << endl;
	cout << " DMAcontroller finished task transmission to memorybuffer\n\n";
	// DMAcontroller 작업 종료 시 알림

	system("pause");
	return;
}
