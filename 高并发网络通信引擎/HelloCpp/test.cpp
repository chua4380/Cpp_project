#include <iostream>
#include <functional>

int funA(int a, int b)
{
	printf("funA\n");
	return 0;
}

using namespace std;
int main()
{
	//funA();
	//std::function< int(int, int) > call(funA);
	//int n = call(0, 2);
	//��Ҫ�ر�ע����ǣ�lambda���ʽ�У�[this]����ǰ���е�thisָ�룬��lambda���ʽӵ�е�ǰ���Ա����ͬ���ķ���Ȩ��
	std::function< int(int) > call;
	int n = 5;
	call = [n](int a) -> int
	{
		printf("hello world!\n");
		return n + 5;
	};
	cout << call(2) << endl;
	system("pause");
	return 0;
}
