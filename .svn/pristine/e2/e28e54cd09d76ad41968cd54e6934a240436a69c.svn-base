#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#include "CELL.hpp"
#include "CELLClient.hpp"
#include "CELLServer.hpp"
#include "INetEvent.hpp"
#include "CELLThread.hpp"
#include "CELLNetwork.hpp"
#include <thread>
#include <mutex>
#include <atomic>


class EasyTcpServer : public INetEvent
{
private:
	//
	CELLThread _thread;
	//消息处理对象，内部会创建线程
	std::vector<CellServer*> _cellServers;
	//每秒消息计时
	CELLTimestamp _tTime;
	//
	SOCKET _sock;
protected:
	//收到消息计数
	std::atomic_int _msgCount;
	//SOCKET recv计数
	std::atomic_int _recvCount;
	//客户端计数
	std::atomic_int _clientCount;
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
		_msgCount = 0;
		_recvCount = 0;
		_clientCount = 0;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}
	//初始化socket/
	SOCKET InitSocket()
	{
		CELLNetwork::Init();
		if (INVALID_SOCKET != _sock)
		{
			CELLLog::Info("<sock=%d>关闭了之前的连接...\n", (int)_sock);
			Close();
		}
		// 1.建立一个socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == _sock) {
			CELLLog::Info("错误，建立socket失败...\n");
		}
		else {
			CELLLog::Info("建立socket成功...\n");
		}
		return _sock;
	}

	//绑定IP和端口号
	int Bind(const char* ip, unsigned short port)
	{
		// 2.bind 绑定用于接受客户端连接的网络端口
		//if (INVALID_SOCKET == _sock)
		//{
		//	InitSocket();
		//}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port); // host to net unsigned short
#ifdef _WIN32
		if (ip) {
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {
			_sin.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("127.0.0.1");
		}
#else
		if (ip) {
			_sin.sin_addr.s_addr = inet_addr(ip);
	}
		else {
			_sin.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
		}
#endif
		int ret = bind(_sock, (sockaddr *)&_sin, sizeof(_sin));
		if (SOCKET_ERROR == ret) {
			CELLLog::Info("错误, 绑定网络端口<%d>失败...\n", port);
		}
		else {
			CELLLog::Info("绑定网络端口<%d>成功...\n", port);
		}
		return ret;
	}

	//监听端口号
	int Listen(int n)  //n表示可以等待的连接数
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret) {
			CELLLog::Info("<socket=%d>错误, 监听网络端口失败...\n", (int)_sock);
		}
		else {
			CELLLog::Info("<socket=%d>监听网络端口成功...\n", (int)_sock);
		}
		return ret;
	}

	//接受客户端连接
	SOCKET Accept()
	{
		// 4.accpet 等待客户端连接
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET cSock = INVALID_SOCKET;
#ifdef _WIN32
		cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);  //接受客户端的连接
#else
		cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
		if (INVALID_SOCKET == cSock)
		{
			CELLLog::Info("<socket=%d>错误，接受到无效的客户端SOCKET\n", (int)_sock);
		}
		else
		{
			//将新客户端分配给客户数量最少的cellServer
			addClientToCellServer(new CellClient(cSock));
			//获取IP地址 inet_ntoa(clientAddr.sin_addr)
		}
		return cSock;
	}

	void addClientToCellServer(CellClient* pClient) {
		//查找客户数量最少的CellServer消息处理对象
		auto pMinserver = _cellServers[0];
		for (auto pCellServer : _cellServers) {
			if (pMinserver->getClientCount() > pCellServer->getClientCount())
				pMinserver = pCellServer;
		}
		pMinserver->addClient(pClient);
	}

	void Start(int nCellServer = 1)
	{
		for (int i = 0; i < nCellServer; i++)
		{
			//创建网络消息处理单元cellserver
			auto ser = new CellServer(i+1);
			_cellServers.push_back(ser);
			//注册网络事件接受对象，this指当前的调用对象的指针，即server
			ser->setEventObj(this);
			//启动消息处理线程
			ser->Start();
		}
		_thread.Start(nullptr,
			[this](CELLThread* pThread) {
				OnRun(pThread);
			}
		);
	}

	//关闭Socket
	void Close()
	{
		CELLLog::Info("EasyTcpServer.Close begin\n");
		_thread.Close();
		if (_sock != INVALID_SOCKET)
		{
			for (auto s : _cellServers)
			{
				delete s;
			}
			_cellServers.clear();
			//关闭套接字socket
#ifdef _WIN32
			closesocket(_sock);
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
		CELLLog::Info("EasyTcpServer.Close end\n");
	}

	//只会被一个线程触发，安全
	virtual void OnNetJoin(CellClient* pClient) {
		_clientCount++;
		//CELLLog::Info("client<%d> join\n", (int)pClient->sockfd());
	}
	//cellServer 4 多个线程触发 不安全
	virtual void OnNetLeave(CellClient* pClient) {
		_clientCount--;
		//CELLLog::Info("client<%d> leve\n", (int)pClient->sockfd());
	}
	//cellServer 4 多个线程触发 不安全
	virtual void onNetMsg(CellServer* pCellServer, CellClient* pClient, DataHeader* header)
	{
		_msgCount++;
	}
	virtual void onNetRecv(CellClient* pClient) {
		_recvCount++;
	}
private:
	//处理网络消息(接受连接)
	void OnRun(CELLThread* pThread)
	{
		while (pThread->isRun()) {
			//输出当前网络连接情况的日志
			time4msg();
			//伯克利套接字 BSD socket
			fd_set fdRead;  // 描述符（socket）集合
			//清理集合 （只是把集合的fd_count置为0）
			FD_ZERO(&fdRead);
			//将 描述符（socket）加入集合
			FD_SET(_sock, &fdRead);
			// nfds是一个整数值是指fd_set集合中所有描述符（socket)的范围，而不是数量
			// 即是所有文件描述符的最大值加一
			timeval t = { 0, 1 };
			int ret = select(int(_sock + 1), &fdRead, 0, 0, &t);
			if (ret < 0)
			{
				CELLLog::Info("EasyTcpServer.onRun accept select exit.\n");
				pThread->Exit();
				break;
			}
			//判断描述符（socket）是否在集合中
			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				//接受客户端连接，并将接受的描述符分发给cellserver做处理
				Accept();
			}
		}
	}

	//计算并输出每秒输出的网络消息
	void time4msg()
	{
		auto t1 = _tTime.getElapsedSecond();
		if (t1 >= 1.0)
		{
			CELLLog::Info("thread<%d>,time<%lf>,socket<%d>, clients<%d>, recv<%d>, msg<%d>\n", (int)_cellServers.size(), t1, (int)_sock, (int)_clientCount, (int)(_recvCount / t1), (int)(_msgCount / t1));
			_recvCount = 0;
			_msgCount = 0;
			_tTime.update();
		}
	}
};

#endif // !_EasyTcpServer_hpp_
