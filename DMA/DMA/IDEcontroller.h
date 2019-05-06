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
	int length; // �� IDEcontroller�� �Ҵ�� task�� ��
	string device; // �� IDEcontroller�� device
	queue<int> controller; // task�� �����ϴ� queue
	static mutex mtx; // �Ӱ迵�� (������ �۾� ȥ���� ����)
	static condition_variable cv; // ���Ǻ��� (�� IDEcontroller �� wait�� notify�� ���)
public:
	static int total_preprocess; // IDEcontroller�� ť�� ����� processing ������ task�� �� ����
	static int total_postprocess; // IDEcontroller���� processing �۾��� �Ϸ�� task�� �� ����
	IDEcontroller(); // ������
	void setdevice(string device); // device ����
	void assign(int tasksize); // task�Ҵ�
	void processing(queue<pair<string, int>> *DMAcon, int tasksize); // taskó��
};