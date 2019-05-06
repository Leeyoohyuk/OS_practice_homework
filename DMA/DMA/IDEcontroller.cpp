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
	unique_lock<mutex> ul(mtx); // mutex 보장
	cv.notify_one(); // 임계영역에 들어오면 wait 상태인 스레드를 깨운다 -> 이 스레드가 wait가 될 경우 다른 스레드가 작업 진행할 수 있도록
	double start = ((double) clock())/CLOCKS_PER_SEC; // Round-Robin 방식을 위한 시간 측정 - (Quantum 시작)
	while (1)
	{
		if (tasksize == total_preprocess)
		{ // 모든 작업이 할당됐다면 루프 탈출 및 스레드 종료
			cout << "\nAll task have been assigned\n\n";
			break;
		}
		if (((double)clock()) / CLOCKS_PER_SEC - start > 0.01)
		{ // Quantum 0.01이 채워진 경우
			cout << "\nQuantum out : Context switch\n\n"; // 
			cv.wait(ul); // 이 스레드를 wait 하고 다른 스레드가 작업 시작
			cv.notify_one(); // 이 스레드의 작업이 재게되었을 때 wait상태의 다른 스레드를 깨운다
			if (tasksize == total_preprocess) // 다른 스레드에서 task assign이 모두 완료된 경우 루프 탈출 및 스레드 종료
			{
				break;
			}
			start = ((double)clock()) / CLOCKS_PER_SEC; // 스레드가 다시 작업을 재게한 시각을 기록한다 (Quantum 시작)
		}
		cout << "PID : <" << device << 'X' << ++length << "> is assigned" << endl;
		controller.push(length); // IDEcontroller의 큐에 작업할당
		++total_preprocess; // preprocess 증가
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
		// 여기서 DMA컨트롤러로 processing 된 작업을 pass 
		//memorybuffer->push(temp);
		cv.wait(ul);
		cv.notify_all();
	}
}