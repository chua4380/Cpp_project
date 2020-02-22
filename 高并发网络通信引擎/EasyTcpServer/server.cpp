#include "EasyTcpServer.hpp"
#include <thread>

class MyServer : public EasyTcpServer
{
public:
	//只会被一个线程触发，安全
	virtual void OnNetJoin(CellClient* pClient) {
		EasyTcpServer::OnNetJoin(pClient);
	}
	//cellServer 4 多个线程触发 不安全
	virtual void OnNetLeave(CellClient* pClient) {
		EasyTcpServer::OnNetLeave(pClient);
	}
	//cellServer 4 多个线程触发 不安全 （重写了父类的onNetMsg方法）
	virtual void onNetMsg(CellServer* pCellServer, CellClient* pClient, DataHeader* header)
	{
		EasyTcpServer::onNetMsg(pCellServer, pClient, header);
		switch (header->cmd) {
		case CMD_LOGIN:
		{
			pClient->resetDTHeart();
			netmsg_Login* login = (netmsg_Login*)header;
			//CELLLog::Info("收到客户端<Socket=%d>请求：CMD_LOGIN数据长度 : %d, userName=%s password=%s\n", (int)cSock, login->dataLength, login->userName, login->PassWord);
			//忽略判断用户名密码是否正确的过程
			netmsg_LoginResult ret;
			if (SOCKET_ERROR == pClient->SendData(&ret))
			{
				CELLLog::Info(" <Socket=%d>发送缓冲区满了，消息没发出去!!!!\n", pClient->sockfd());
			}

			//用任务的形式发送消息
			//netmsg_LoginResult* ret = new netmsg_LoginResult();
			//pCellServer->addSendTask(pClient, ret);
		}  //接受消息---处理 发送（有数据传送）   生产者 数据缓冲区 消费者
		break;
		case CMD_LOGOUT:
		{
			netmsg_Logout* logout = (netmsg_Logout*)header;
			//CELLLog::Info("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度 : %d, userName=%s\n", (int)cSock, logout->dataLength, logout->userName);
			//忽略判断用户名密码是否正确的过程
			//netmsg_LogoutResult ret;
			//pClient->SendData(&ret);
		}
		break;
		case CMD_C2S_HEART:
		{
			pClient->resetDTHeart();
			netmsg_s2c_Heart ret;
			pClient->SendData(&ret);
		}
		break;
		default:
		{
			CELLLog::Info("收到客户端<Socket=%d>未定义消息，数据长度 : %d\n", (int)pClient->sockfd(), header->dataLength);
			//DataHeader ret;
			//SendData(cSock, &ret);
		}
		break;
		}
	}

	virtual void onNetRecv(CellClient* pClient) {
		EasyTcpServer::onNetRecv(pClient);
	}
private:

};

int main()
{
	CELLLog::Instance().setLogPath("serverLog.txt", "w");
	MyServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);
	server.Start(4);

	////启动线程
	//std::thread t1(cmdThread);
	//t1.detach();

	while (true) {
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "e")) {
			server.Close();
			CELLLog::Info("退出cmdThread线程\n");
			break;
		}

		else {
			CELLLog::Info("不支持的命令\n");
		}
	}

	server.Close();
	CELLLog::Info("已退出。\n");
	server.Close();
	//while (true)
	//{
	//	std::chrono::seconds t(1);
	//	std::this_thread::sleep_for(t);
	//}
	//getchar();
	system("pause");
	return 0;
}
