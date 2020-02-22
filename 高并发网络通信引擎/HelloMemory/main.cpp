#include"Alloctor.h"
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>//锁
#include <memory>
#include "CELLTimestamp.hpp"
#include "CELLObjectPool.hpp"
using namespace std;
//原子操作   原子 分子 
mutex m;

class classA :public ObjectPoolBase<classA, 5>
{
public:
	classA(int n)
	{
		num = n;
		//printf("classA\n");
	}
	~classA()
	{
		//printf("~classA\n");
	}
public:
	int num = 0;
	char arr[1000];
};
class classB :public ObjectPoolBase<classB, 10>
{
public:
	classB(int m, int n)
	{
		num = m * n;
		printf("classB\n");
	}
	~classB()
	{
		printf("~classB\n");
	}
public:
	int num = 0;

};


const int tCount = 4;
const int mCount = 10000;
const int nCount = mCount / tCount;

void workFun(int index)
{
	classA* data[nCount];
	for (size_t i = 0; i < nCount; i++)
	{
		data[i] = classA::createObject(6);
	}
	for (size_t i = 0; i < nCount; i++)
	{
		classA::destoryObject(data[i]);
	}

}//抢占式

/*
void fun(shared_ptr<classA>& pA)
{
	pA->num++;
}
void fun(classA* pA)
{
	pA->num++;
}
*/

int main()
{
	/*
	thread t[tCount];
	for (int n = 0; n < tCount; n++)
	{
		t[n] = thread(workFun, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < tCount; n++)
	{
		t[n].join();
		//t[n].detach();
	}
	cout << tTime.getElapsedTimeInMilliSec() << endl;
	cout << "Hello,main thread." << endl;
	*/

	/*
	int* a = new int;
	*a = 10;
	//delete a;
	printf("a = %d\n", *a);
	delete a;
	//C++智能指针的一种
	shared_ptr<int> b = make_shared<int>();
	*b = 100;
	printf("b = %d\n", *b);
	*/

	/*
	{
		shared_ptr<classA> a = make_shared<classA>(100);
		CELLTimestamp tTime;
		for (int i = 0; i < 100000000; i++) 
		{
			fun(a);
		}
		cout << tTime.getElapsedTimeInMilliSec() << endl;
	}
	{
		classA* a = new classA(100);
		CELLTimestamp tTime;
		for (int i = 0; i < 100000000; i++)
		{
			fun(a);
		}
		cout << tTime.getElapsedTimeInMilliSec() << endl;
	}
	*/

	/*
	classA * a1 = new classA(5);
	delete a1;

	classA* a2 = classA::createObject(6);
	classA::destoryObject(a2);

	classB * b1 = new classB(5, 3);
	delete b1;

	classB* b2 = new classB(2, 3);
	classB::destoryObject(b2);
	*/

	classB* b1 = new classB(2,3);
	printf("-----------1--------\n");
	{
		std::shared_ptr<classA> s1 = make_shared<classA>(5);

	}
	printf("-----------2--------\n");
	{
		std::shared_ptr<classA> s2(new classA(5));
	}
	printf("-----------3--------\n");
	classA* a1 = new classA(5);
	delete a1;
	delete b1;
	printf("-----------4--------\n");
	//getchar();
	system("pause");

	return 0;
}