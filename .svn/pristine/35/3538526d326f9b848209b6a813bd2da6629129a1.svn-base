#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include "CELLTimestamp.hpp"
using namespace std;
//原子操作
mutex m;
const int tCount = 4;
atomic_int sum = 0;
void workFun(int index)
{
	
	for (int i = 0; i < 20000000; i++) {
		//自解锁
		//lock_guard<mutex> lg(m);
		//m.lock();
		//临界区域开始
		sum++;
		//临界区域结束
		//m.unlock();

	}

}

int main()   //主线程的入库函数
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