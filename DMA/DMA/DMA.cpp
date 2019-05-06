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
	string device[5] = { "keyboard", "mouse", "monitor", "printer", "disk" }; // �׽�ŷ device type
	int task_size = size(); // task�� ����
	int num_of_device = type(); // type�� ����
	pair<int, string> task; // �� task�� ���� ���� ���� (�� ��° task����, � type�� device���� ���� ����)

	IDEcontroller *idecon = new IDEcontroller[num_of_device]; // IDEcontroller ��ü �����Ҵ�
	int *select_device = new int[num_of_device]; // ����� device type�� ����
	thread *ide = new thread[num_of_device*2]; // ����� devcie ���� * 2 ��ŭ�� ������ �Ҵ� -> �Ҵ�� ó���� �и�
	
	cout << "Select task device\n"; 
	cout << "keyboard --- 0\n";
	cout << "mouse ------ 1\n";
	cout << "monitor ---- 2\n";
	cout << "printer ---- 3\n";
	cout << "disk ------- 4\n";
	cout << "Insert " << num_of_device << "type numbers (Multiple inputs on one line) : ";
	// ����� device�� �����Ѵ�

	for (int i = 0; i < num_of_device; i++)
	{
		cin >> select_device[i];
		idecon[i].setdevice(device[select_device[i]]);
	} // IDEcontroller�� device type�� �����Ѵ�

	queue<pair<string, int>> *memorybuffer = new queue<pair<string, int>>;
	for(int i = 0; i<num_of_device; i++)
	{
		ide[i] = thread(&IDEcontroller::assign, &idecon[i], task_size); 
		ide[i+num_of_device] = thread(&IDEcontroller::processing, &idecon[i], memorybuffer, task_size);
	} // ��Ƽ�������� �̿��Ͽ� assign - processing �۾� ����
	for (int i = 0; i < num_of_device * 2; i++)
	{
		ide[i].join();
	} // ��� �۾� ���� �� �˸�
	cout << "\n All task on IDEcontroller have been operated";
	return;
}
