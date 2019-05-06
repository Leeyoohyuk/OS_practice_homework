#include <iostream>
#include "IDEcontroller.h"
#include <thread>
#include <Windows.h>
#include <string>
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
	cout << "Please enter the number of tasks you want to task : ";
	cin >> tasksize;
	return tasksize;
}

int type()
{
	int num_of_device = 0;
	cout << "Please enter the number of type you want to task : ";
	cin >> num_of_device;
	return num_of_device;
}

void task_processing()
{
	string device[5] = { "keyboard", "mouse", "monitor", "printer", "disk" }; // 테스킹 device type
	int task_size = size(); // task의 개수
	int num_of_device = type(); // type의 개수
	pair<int, string> task; // 한 task에 대한 간단 정보 (몇 번째 task인지, 어떤 type의 device인지 정보 포함)

	IDEcontroller *idecon = new IDEcontroller[num_of_device]; // IDEcontroller 객체 동적할당
	int *select_device = new int[num_of_device]; // 사용할 device type의 개수
	thread *ide = new thread[num_of_device*2]; // 사용할 devcie 개수 * 2 만큼의 스레드 할당 -> 할당과 처리의 분리
	
	cout << "Select task device\n"; 
	cout << "keyboard --- 0\n";
	cout << "mouse ------ 1\n";
	cout << "monitor ---- 2\n";
	cout << "printer ---- 3\n";
	cout << "disk ------- 4\n";
	cout << "Insert " << num_of_device << "type numbers (Multiple inputs on one line) : ";
	// 사용할 device를 선택한다

	for (int i = 0; i < num_of_device; i++)
	{
		cin >> select_device[i];
		idecon[i].setdevice(device[select_device[i]]);
	} // IDEcontroller에 device type을 저장한다

	queue<pair<string, int>> *memorybuffer = new queue<pair<string, int>>;
	for(int i = 0; i<num_of_device; i++)
	{
		ide[i] = thread(&IDEcontroller::assign, &idecon[i], task_size); 
		ide[i+num_of_device] = thread(&IDEcontroller::processing, &idecon[i], memorybuffer, task_size);
	} // 멀티스레딩을 이용하여 assign - processing 작업 시작
	for (int i = 0; i < num_of_device * 2; i++)
	{
		ide[i].join();
	} // 모든 작업 종료 시 알림
	cout << "\n All task on IDEcontroller have been operated";
	return;
}
