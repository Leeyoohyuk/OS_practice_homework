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
	random_device ranum;
	string device[5] = { "keyboard", "mouse", "monitor", "printer", "disk" }; // �׽�ŷ device type
	bool flag[5] = { 0 };
	int task_size = size(); // task�� ����
	int num_of_device = type(); // type�� ����

	IDEcontroller *idecon = new IDEcontroller[num_of_device]; // IDEcontroller ��ü �����Ҵ�
	thread *ide = new thread[num_of_device*2]; // ����� devcie ���� * 2 ��ŭ�� ������ �Ҵ� -> �Ҵ�� ó���� �и�
	queue<pair<string, int>> *DMAcon = new queue<pair<string, int>>;
	for (int i = 0; i < num_of_device; i++)
	{
		int randnum;
		do {
			randnum = ranum() % 5;
		} while (flag[randnum]);
		flag[randnum] = true;

		idecon[i].setdevice(device[randnum]);
	} // IDEcontroller�� device type�� �����Ѵ�

	for(int i = 0; i<num_of_device; i++)
	{
		ide[i] = thread(&IDEcontroller::assign, &idecon[i], task_size); 
		ide[i+num_of_device] = thread(&IDEcontroller::processing, &idecon[i], DMAcon, task_size);
	} // ��Ƽ�������� �̿��Ͽ� assign - processing �۾� ����
	for (int i = 0; i < num_of_device * 2; i++)
	{
		ide[i].join();
	} // ��� �۾� ���� �� �˸�
	cout << "All task on IDEcontroller have been processed\n";
	cout << "\n Start assigning on the memory buffer\n\n";
	while (DMAcon->size())
	{
		cout << DMAcon->front().first << 'X' << DMAcon->front().second << " have been assigned on the memory buffer\n";
		DMAcon->pop();
	}
	cout << endl;

	return;
}
