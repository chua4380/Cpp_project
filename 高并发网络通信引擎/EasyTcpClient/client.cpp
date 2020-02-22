#include "EasyTcpClient.hpp"
#include <thread>
#include "CELLTimestamp.hpp"
#include <atomic>

bool g_bRun = true;
void cmdThread()
{
	while (true) {
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit")) {
			//client->Close();
			g_bRun = false;
			printf("退出cmdThread线程\n");
			break;
		}
		//else if (0 == strcmp(cmdBuf, "login")) {
		//	netmsg_Login login;
		//	strcpy(login.userName, "lioney");
		//	strcpy(login.PassWord, "lioney");
		//	client->SendData(&login);
		//}
		//else if (0 == strcmp(cmdBuf, "logout")) {
		//	netmsg_Logout logout;
		//	strcpy(logout.userName, "lioney");
		//	client->SendData(&logout);
		//}
		else {
			printf("不支持的命令\n");
		}
	}

}
//客户端数量
const int cCount = 1000;
//发送线程数量
const int tCount = 4;
//客户端数组
EasyTcpClient* client[cCount];
std::atomic_int sendCount = 0;
std::atomic_int readyCount = 0;

void recvThread(int begin, int end)
{
	//CELLTimestamp t;
	while (g_bRun)
	{
		for (int n = begin; n < end; n++) {
			//if (t.getElapsedSecond() > 2 && n % 2 == 0)
			//	continue;
			client[n]->OnRun();
		}
	}
}

void sendThread(int id) 
{
	printf("thread<%d>,start\n", id);
	//4个线程
	int c = cCount / tCount;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int n = begin; n < end; n++) {
		client[n] = new EasyTcpClient();
	}
	for (int n = begin; n < end; n++) {
		client[n]->Connect("10.109.242.110", 4567); //10.109.242.252. 192.168.5.132
	}
	//心跳检测 死亡计时
	printf("thread<%d>,Connect<begin=%d, end=%d>\n",id, begin, end);

	readyCount++;
	while (readyCount < tCount) {
		//等待其他线程准备好发送数据
		std::chrono::milliseconds t(1);
		std::this_thread::sleep_for(t);
	}

	//
	std::thread t1(recvThread, begin, end);
	t1.detach();

	netmsg_Login login[10];
	for (int n = 0; n < 10; n++)
	{
		strcpy(login[n].userName, "lioney");
		strcpy(login[n].PassWord, "lioneypw");
	}
	const int nLen = sizeof(login);
	while (g_bRun)
	{
		for (int n = begin; n < end; n++)
		{
			if (SOCKET_ERROR != client[n]->SendData(login, nLen))
			{
				sendCount++;
			}
		}
		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);
	}

	for (int n = begin; n < end; n++) {
		client[n]->Close();
		delete client[n];
	}
	printf("thread<%d>,exit\n", id);
}

int main()
{
	//启动UI线程
	std::thread t1(cmdThread);
	t1.detach();

	//启动发送线程
	for (int n = 0; n < tCount; n++)
	{
		std::thread t1(sendThread, n + 1);
		t1.detach();
	}

	CELLTimestamp tTime;
	while (g_bRun) {
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0) {
			printf("thread<%d>, clients<%d>, time<%lf>, send<%d>\n", tCount, cCount, t, (int)(sendCount/t));
			sendCount = 0;
			tTime.update();
		}
		Sleep(1);
	}
	printf("已退出。\n");
	//getchar();
	return 0;
}