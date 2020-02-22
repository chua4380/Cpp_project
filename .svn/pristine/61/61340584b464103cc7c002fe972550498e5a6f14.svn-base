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
	//需要特别注意的是：lambda表达式中：[this]捕获当前类中的this指针，让lambda表达式拥有当前类成员函数同样的访问权限
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
