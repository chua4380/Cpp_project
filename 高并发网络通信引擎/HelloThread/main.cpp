#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include "CELLTimestamp.hpp"
using namespace std;
//ԭ�Ӳ���
mutex m;
const int tCount = 4;
atomic_int sum = 0;
void workFun(int index)
{
	
	for (int i = 0; i < 20000000; i++) {
		//�Խ���
		//lock_guard<mutex> lg(m);
		//m.lock();
		//�ٽ�����ʼ
		sum++;
		//�ٽ��������
		//m.unlock();

	}

}

int main()   //���̵߳���⺯��
{

	thread t[tCount];
	for (int n = 0; n < tCount; n++)
	{
		t[n] = thread(workFun, n);
		//t[n].detach();
	}
	CELLTimestamp tTime;
	for (int i = 0; i < tCount; i++)
	{
		t[i].join();
	}
	cout << tTime.getElapsedTimeInMilliSec() << ", sum = " << sum << endl;
	tTime.update();
	sum = 0;
	for (int i = 0; i < 80000000; i++) {
		sum++;
	}
	cout << tTime.getElapsedTimeInMilliSec() << ", sum = " << sum << endl;
	//t.join();
	cout << "hello, main thread.\n";
	getchar();
	return 0;
}