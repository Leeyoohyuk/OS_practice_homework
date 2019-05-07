#include "IDEcontroller.h"
#include <time.h>
#include <Windows.h>
#include <chrono>
//#include <condition_variable>
using namespace std;

// static 초기화
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
	this->device = device; // 디바이스의 이름을 받아서 IDEcontroller 세팅
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
			cout << "\n All task have been assigned\n\n";
			break;
		}
		if (((double)clock()) / CLOCKS_PER_SEC - start > 0.01)
		{ // Quantum 0.01이 채워진 경우
			cout << "\n Quantum out : Context switch\n\n";
			cv.wait(ul); // 이 스레드를 wait 하고 다른 스레드가 작업 시작
			cv.notify_one(); // 이 스레드의 작업이 재게되었을 때 wait상태로 들어간 스레드를 깨운다
			if (tasksize == total_preprocess) // 다른 스레드에서 task assign이 모두 완료된 경우 루프 탈출 및 스레드 종료
			{
				break;
			}
			start = ((double)clock()) / CLOCKS_PER_SEC; // 스레드가 다시 작업을 재게한 시각을 기록한다 (Quantum 시작)
		}
		cout << " PID : <" << device << 'X' << ++length << "> is assigned" << endl;
		controller.push(length); // IDEcontroller의 큐에 작업할당
		++total_preprocess; // 총 할당 프로세스 증가
	}
}

void IDEcontroller::processing(queue<pair<string,int>> *DMAcon, int tasksize)
{
	unique_lock<mutex> ul(mtx); // mutex 보장
	cv.notify_one(); // 임계영역에 들어오면 wait 상태인 스레드를 깨운다 -> 이 스레드가 wait가 될 경우 다른 스레드가 작업 진행할 수 있도록
	while (1)
	{
		if (controller.size() == 0) // 컨트롤러에 할당된 작업이 없다면
		{
			cout << "\n There isn't task associated with " << device << endl << endl;
			while (controller.size() == 0)
			{
				if (tasksize == total_postprocess) // 컨트롤러에 작업이 없고 모든 프로세스의 처리가 완료되었다면
				{
					break; // 루프를 탈출
				}
				cv.wait(ul); // 그렇지 않으면 컨트롤러에 process가 새로 assign될 수 있기때문에 wait상태로 들어간다
				cv.notify_all(); // 작업 재게시 wait 스레드를 깨운다
				if (tasksize == total_postprocess) // wait에서 깨어났을 때 다른 스레드에서 모든 process가 처리되었다면
				{
					break; // 루프를 탈출
				}
			}
		}
		if (tasksize == total_postprocess) // 작업이 전부 처리된 경우 이 루프도 탈출시키고 스레드를 종료한다.
		{
			break;
		}
		cout << " Process : " << device << 'x' << controller.front() << " in progress. \n";
		total_postprocess++; 
		pair<string, int> temp;
		temp.first = device;
		temp.second = controller.front();
		controller.pop(); 
		DMAcon->push(temp); 
		// 1. DMA 컨트롤러로 processing 된 작업을 전달 2. 프로세싱된 작업의 수 증가 3. 컨트롤러에서 작업 제거
		cv.wait(ul); // 한 번에 하나의 process만 처리해야하기 때문에 한 번의 processing 진행되면 다른 스레드가 작업
		cv.notify_all(); // 작업 재게시 wait 스레드를 깨운다
	}
}