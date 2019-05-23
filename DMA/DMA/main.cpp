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
	while (tasksize < 500) // 500 �̸��� type�� �Է½� ���Է� �޵��� ��
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
	while (num_of_device < 3) // 3 �̸��� type�� �Է½� ���Է� �޵��� ��
	{
		cout << " Please enter a number of more than three : ";
		cin >> num_of_device;
	}
	return num_of_device;
}

void task_processing()
{
	random_device ranum;
	string device[5] = { "keyboard", "mouse", "monitor", "printer", "disk" }; // �׽�ŷ device type
	bool flag[5] = { 0 };
	int task_size = size(); // task�� ����
	int num_of_device = type(); // type�� ����

	IDEcontroller *idecon = new IDEcontroller[num_of_device]; // IDEcontroller ��ü �����Ҵ�
	thread *ide = new thread[num_of_device * 2]; // ����� devcie ���� * 2 ��ŭ�� ������ �Ҵ� -> �Ҵ�� ó���� �и�
	queue<pair<string, int>> *DMAcontroller = new queue<pair<string, int>>;
	for (int i = 0; i < num_of_device; i++)
	{
		int randnum;
		do {
			randnum = ranum() % 5;
		} while (flag[randnum]);
		flag[randnum] = true;
		idecon[i].setdevice(device[randnum]);
	} // IDEcontroller�� �������� device ����

	for (int i = 0; i < num_of_device; i++)
	{
		ide[i] = thread(&IDEcontroller::assign, &idecon[i], task_size);
		ide[i + num_of_device] = thread(&IDEcontroller::processing, &idecon[i], DMAcontroller, task_size);
	} // ��Ƽ�������� �̿��Ͽ� assign - processing �۾� ����
	for (int i = 0; i < num_of_device * 2; i++)
	{
		ide[i].join();
	} // ��� �۾� ���� �� �˸�
	cout << " All task on IDEcontroller have been processed\n";
	cout << "\n Start assigning on the memory buffer\n\n";
	Sleep(5000); // ���Ȯ�� �� sleep

	while (DMAcontroller->size())
	{
		cout << ' ' << DMAcontroller->front().first << 'X' << DMAcontroller->front().second << " have been transfered on the memory buffer\n";
		DMAcontroller->pop(); // �޸� ���ۿ� ����
	}
	cout << endl;
	cout << " DMAcontroller finished task transmission to memorybuffer\n\n";
	// DMAcontroller �۾� ���� �� �˸�

	system("pause");
	return;
}
