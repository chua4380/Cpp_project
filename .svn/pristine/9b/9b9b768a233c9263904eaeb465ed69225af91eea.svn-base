#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

//#pragma comment(lib, "ws2_32.lib")   加载动态链接库

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
    // 启动windows socket 2.x网络环境
	WSAStartup(ver, &dat);

	WSACleanup();
	return 0;
}