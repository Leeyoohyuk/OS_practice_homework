#pragma once
#include <string>
#include <queue>
#include <iostream>
#include <mutex>
#include <condition_variable>
using namespace std;

class IDEcontroller
{
private:
	int length; // 이 IDEcontroller에 할당된 task의 수
	string device; // 이 IDEcontroller의 device
	queue<int> controller; // task를 저장하는 queue
	static mutex mtx; // 임계영역 (스레드 작업 혼선을 없앰)
	static condition_variable cv; // 조건변수 (각 IDEcontroller 간 wait및 notify에 사용)
public:
	static int total_preprocess; // IDEcontroller의 큐에 저장된 processing 이전의 task의 총 갯수
	static int total_postprocess; // IDEcontroller에서 processing 작업이 완료된 task의 총 갯수
	IDEcontroller(); // 생성자
	void setdevice(string device); // device 세팅
	void assign(int tasksize); // task할당
	void processing(queue<pair<string, int>> *DMAcon, int tasksize); // task처리
};