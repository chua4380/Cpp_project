#ifndef _CELL_SERVER_HPP
#define _CELL_SERVER_HPP   

#include "CELL.hpp"
#include "INetEvent.hpp"
#include "CELLClient.hpp"
#include "CELLThread.hpp"

#include <vector>
#include <unordered_map>

//网络消息接受服务类
class CellServer
{
public:
	CellServer(int id)
	{
		_id = id;
		_pNetEvent = nullptr;
		_taskServer.serverId = id;
	}
	~CellServer()
	{
		CELLLog::Info("CellServer%d.~CellServer exit begin\n", _id);
		Close();
		CELLLog::Info("CellServer%d.~CellServer exit end\n", _id);
	}

	void setEventObj(INetEvent* event)
	{
		_pNetEvent = event;
	}

	//关闭Socket
	void Close()
	{
		CELLLog::Info("CellServer%d.Close exit begin\n", _id);
		_taskServer.Close();
		_thread.Close();
		CELLLog::Info("CellServer%d.Close exit end\n", _id);
	}

	//处理网络消息
	void OnRun(CELLThread* pThread)
	{
		while (pThread->isRun()) {
			//从缓冲队列里取出客户数据
			if (_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pClient : _clientsBuff)
				{
					_clients[pClient->sockfd()] = pClient;
					pClient->serverId = _id;
					if (_pNetEvent)
						_pNetEvent->OnNetJoin(pClient);
				}
				_clientsBuff.clear();
				_clients_change = true;
			}
			//如果没有需要处理的客户端，就跳过
			if (_clients.empty()) {
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				// 更新旧的时间戳
				_oldTime = CELLTime::getNowInMilliSec();
				continue;
			}

			//伯克利套接字 BSD socket
			fd_set fdRead;  // 描述符（socket）集合
			fd_set fdWrite;
			//fd_set fdExc;

			if (_clients_change)
			{
				_clients_change = false;
				//清理集合 （只是把集合的fd_count置为0）
				FD_ZERO(&fdRead);
				//将 描述符（socket）加入集合
				_maxSock = _clients.begin()->second->sockfd();
				//将新加入的客户端加到可读集合里面查询有没有数据需要接收
				for (auto iter : _clients)
				{
					FD_SET(iter.second->sockfd(), &fdRead);
					if (_maxSock < iter.second->sockfd())
					{
						_maxSock = iter.second->sockfd();
					}
				}
				memcpy(&_fdRead_bak, &fdRead, sizeof(fd_set));
			}
			else {
				memcpy(&fdRead, &_fdRead_bak, sizeof(fd_set));
			}

			//将之前的fdRead集合中的fd_set加入fdWrite集合中，
			memcpy(&fdWrite, &_fdRead_bak, sizeof(fd_set));
			//memcpy(&fdExc, &_fdRead_bak, sizeof(fd_set));

			// nfds是一个整数值是指fd_set集合中所有描述符（socket)的范围，而不是数量
			// 即是所有文件描述符的最大值加一
			timeval t = { 0,1 };
			//查询处于活动状态的fd_set是否可读或可写
			int ret = select(int(_maxSock + 1), &fdRead, &fdWrite, NULL, &t);
			if (ret < 0)
			{
				CELLLog::Info("CellServer%d.OnRun.select Error\n", _id);
				pThread->Exit();
				break;
			}
			//else if (ret = 0)
			//{
			//	continue;
			//}
			ReadData(fdRead);
			WriteData(fdWrite);
			//WriteData(fdExc);

			//CELLLog::Info("CELLServer%d.OnRun.select: fdRead=%d, fdWrite=%d\n", _id, fdRead.fd_count, fdWrite.fd_count);
			//if (fdExc.fd_count > 0) {
			//	CELLLog::Info("###fdExc=%d\n", fdExc.fd_count);
			//}
			//心跳检测，删掉已经断开的客户端，防止“死链”的出现
			CheckTime();
		}
		CELLLog::Info("CellServer%d.onRun exit\n", _id);
	}

	void CheckTime() {
		// 新的时间戳
		auto nowTime = CELLTime::getNowInMilliSec();
		auto dt = nowTime - _oldTime;
		_oldTime = nowTime;
		for (auto iter = _clients.begin(); iter != _clients.end();)
		{
			//心跳检测
			if (iter->second->checkHeart(dt))
			{
				onClientLeave(iter->second);
				auto iterOld = iter++;
				_clients.erase(iterOld);
				continue;
			}
			////定时发送数据(阻塞情况下很有效，异步情况下要根据业务逻辑判断)
			//iter->second->checkSend(dt);
			iter++;
		}
	}

	void onClientLeave(CellClient* pClient)
	{
		if (_pNetEvent)
			_pNetEvent->OnNetLeave(pClient);
		_clients_change = true;
		delete pClient;
	}

	void WriteData(fd_set& fdWrite)
	{
		//循环处理要数据进来的客户端（fd_set在linux和windows中实现不同）
#ifdef _WIN32
		for (int n = 0; n < (int)fdWrite.fd_count; n++)
		{
			auto iter = _clients.find(fdWrite.fd_array[n]);
			if (iter != _clients.end())
			{
				if (-1 == iter->second->SendDataReal())
				{
					onClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}
#else
		std::vector<CellClient*> temp;
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (FD_ISSET(iter->second->sockfd(), &fdWrite))
			{
				if (-1 == iter->second->SendDataReal())
				{
					onClientLeave(iter->second);
					auto iterOld = iter++;
					_clients.erase(iterOld);
					continue;
				}
			}
			iter++;
		}
#endif
	}

	void ReadData(fd_set& fdRead)
	{
		//循环处理要数据进来的客户端（fd_set在linux和windows中实现不同）
#ifdef _WIN32
		for (int n = 0; n < (int)fdRead.fd_count; n++)
		{
			auto iter = _clients.find(fdRead.fd_array[n]);
			if (iter != _clients.end())
			{
				if (-1 == RecvData(iter->second))
				{
					onClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
			else {
				CELLLog::Info("error. if (iter != _clients.end())\n");
			}
		}
#else
		std::vector<CellClient*> temp;
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (FD_ISSET(iter->second->sockfd(), &fdRead))
			{
				if (-1 == RecvData(iter->second))
				{
					onClientLeave(iter->second);
					iter = _clients.erase(iter);
					continue;
				}
			}
			iter++;
		}
#endif
	}

	//接受缓冲区
	//char _szRecv[RECV_BUFF_SIZE] = {};
	//接收数据 处理粘包 拆分包
	int RecvData(CellClient* pClient)
	{
		//接收客户端数据
		int nLen = pClient->RecvData();

		//CELLLog::Info("nLen=%d\n", nLen);
		if (nLen <= 0)
		{
			return -1;
		}
		//触发<接收到网络数据>事件
		_pNetEvent->onNetRecv(pClient);
		//循环 判断是否有消息需要处理
		while (pClient->hasMsg())
		{
			//处理网络消息
			OnNetMsg(pClient, pClient->front_msg());
			//移除消息缓冲区最前的一条数据
			pClient->pop_front_msg();
		}
		return 0;
	}
	//响应网络消息
	virtual void OnNetMsg(CellClient* pClient, DataHeader* header)
	{
		_pNetEvent->onNetMsg(this, pClient, header);
	}

	void addClient(CellClient* pClient)
	{
		std::lock_guard<std::mutex> lg(_mutex);
		//_mutex.lock();
		_clientsBuff.push_back(pClient);
		//_mutex.unlock();
	}

	void Start()
	{
		_taskServer.Start();
		//启动时向_thread注册三个匿名的回调函数
		_thread.Start(
			// onCreate
			nullptr, 
			// onRun
			[this](CELLThread* pThread) {
				OnRun(pThread);
			},
			// onDestroy
			[this](CELLThread* pThread) {
				ClearClients();
			}
		);

	}

	size_t getClientCount()
	{
		return _clients.size() + _clientsBuff.size();
	}

	//void addSendTask(CellClient* pClient, DataHeader* header)
	//{
	//	//CellSendMsg2ClientTask* task = new CellSendMsg2ClientTask(pClient, header);
	//	_taskServer.addTask([pClient, header]() {
	//		pClient->SendData(header);
	//		delete header; });
	//}
private:
	void ClearClients()
	{
		for (auto iter : _clients)
		{
			delete iter.second;
		}
		_clients.clear();

		for (auto iter : _clientsBuff)
		{
			delete iter;
		}
		_clientsBuff.clear();
	}
private:
	//正式客户队列
	std::unordered_map<SOCKET, CellClient*> _clients;
	//缓冲客户队列
	std::vector<CellClient*> _clientsBuff;
	//缓冲队列的锁
	std::mutex _mutex;
	//网络事件对象
	INetEvent* _pNetEvent;
	//
	CellTaskServer _taskServer;
	//备份客户socket fd_set
	fd_set _fdRead_bak;
	SOCKET _maxSock;
	// 旧的时间戳
	time_t _oldTime = CELLTime::getNowInMilliSec();
	//
	CELLThread _thread;
	//
	int _id = -1;
	//客户是否有变化
	bool _clients_change = true;

};

#endif // !_CELL_SERVER_HPP
