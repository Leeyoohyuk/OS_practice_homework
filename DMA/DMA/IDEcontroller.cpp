#include "IDEcontroller.h"
#include <time.h>
#include <Windows.h>
#include <chrono>
//#include <condition_variable>
using namespace std;

mutex IDEcontroller::mtx;
condition_variable IDEcontroller::cv;
int IDEcontroller::total_preprocess = 0;
int IDEcontroller::total_postprocess = 0;

IDEcontroller::IDEcontroller()
{
	length = 0;
}

void IDEcontroller::setdevice(string device)
{
	this->device = device;
}

void IDEcontroller::assign(int tasksize)
{
	unique_lock<mutex> ul(mtx); // mutex ����
	cv.notify_one(); // �Ӱ迵���� ������ wait ������ �����带 ����� -> �� �����尡 wait�� �� ��� �ٸ� �����尡 �۾� ������ �� �ֵ���
	double start = ((double) clock())/CLOCKS_PER_SEC; // Round-Robin ����� ���� �ð� ���� - (Quantum ����)
	while (1)
	{
		if (tasksize == total_preprocess)
		{ // ��� �۾��� �Ҵ�ƴٸ� ���� Ż�� �� ������ ����
			cout << "\nAll task have been assigned\n\n";
			break;
		}
		if (((double)clock()) / CLOCKS_PER_SEC - start > 0.01)
		{ // Quantum 0.01�� ä���� ���
			cout << "\nQuantum out : Context switch\n\n"; // 
			cv.wait(ul); // �� �����带 wait �ϰ� �ٸ� �����尡 �۾� ����
			cv.notify_one(); // �� �������� �۾��� ��ԵǾ��� �� wait������ �ٸ� �����带 �����
			if (tasksize == total_preprocess) // �ٸ� �����忡�� task assign�� ��� �Ϸ�� ��� ���� Ż�� �� ������ ����
			{
				break;
			}
			start = ((double)clock()) / CLOCKS_PER_SEC; // �����尡 �ٽ� �۾��� ����� �ð��� ����Ѵ� (Quantum ����)
		}
		cout << "PID : <" << device << 'X' << ++length << "> is assigned" << endl;
		controller.push(length); // IDEcontroller�� ť�� �۾��Ҵ�
		++total_preprocess; // preprocess ����
	}
}

void IDEcontroller::processing(queue<pair<string, int>> *memorybuffer, int tasksize)
{
	unique_lock<mutex> ul(mtx);
	cv.notify_one();
	while (1)
	{
		if (controller.size() == 0)
		{
			cout << "\nThere isn't task associated with " << device << endl << endl;
			while (controller.size() == 0)
			{
				if (tasksize == total_postprocess)
				{
					break;
				}
				cv.wait(ul);
				cv.notify_all();
				if (tasksize == total_postprocess)
				{
					break;
				}
			}
		}
		if (tasksize == total_postprocess)
		{
			break;
		}
		cout << "Process : " << device << 'x' << controller.front() << " in progress. \n";
		total_postprocess++;
		pair<string, int> temp;
		temp.first = device;
		temp.second = controller.front();
		controller.pop();
		// ���⼭ DMA��Ʈ�ѷ��� processing �� �۾��� pass 
		//memorybuffer->push(temp);
		cv.wait(ul);
		cv.notify_all();
	}
}