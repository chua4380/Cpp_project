#ifndef _CellClient_hpp_
#define _CellClient_hpp_

#include "CELL.hpp"
#include "CELLBuffer.hpp"
//客户端心跳检测死亡计时
#define CLIENT_HEART_DEAD_TIME 60000
//在指定时间内清空发送缓冲区
#define CLIENT_SEND_BUFF_TIME 200
//客户端数据类型
class CellClient
{
public:
	int id = -1;
	//所属serverId
	int serverId = -1;
public:
	CellClient(SOCKET sockfd = INVALID_SOCKET) : _sendBuf(SEND_BUFF_SIZE), _recvBuf(SEND_BUFF_SIZE)
	{
		static int n = 1;
		id = n++;
		_sockfd = sockfd;

		resetDTHeart();
		resetDTSend();
	}

	~CellClient()
	{
		CELLLog::Info("s = %d CellTaskServer%d.~CellClient \n", serverId, id);
		if (INVALID_SOCKET != _sockfd)
		{
#ifdef _WIN32
			closesocket(_sockfd);
#else
			close(_sockfd);
#endif
			_sockfd = INVALID_SOCKET;
		}
	}

	SOCKET sockfd()
	{
		return _sockfd;
	}
	int RecvData()
	{
		return _recvBuf.read4socket(_sockfd);
	}

	bool hasMsg()
	{
		return _recvBuf.hasMsg();
	}
	DataHeader* front_msg()
	{
		return (DataHeader*)_recvBuf.data();
	}

	void pop_front_msg()
	{
		if(hasMsg())
			_recvBuf.pop(front_msg()->dataLength);
	}

	//定量给指定Socket发送数据
	int SendData(DataHeader* header)
	{
		if (_sendBuf.push((const char*)header, header->dataLength)) {
			return header->dataLength;
		}

		return SOCKET_ERROR;
	}

	//立即将发送缓冲区的数据发送给客户端
	int SendDataReal()
	{
		resetDTSend();
		return _sendBuf.write2socket(_sockfd);
	}

	void resetDTHeart()
	{
		_dtHeart = 0;
	}

	//
	void resetDTSend()
	{
		_dtSend = 0;
	}
	//心跳检测
	bool checkHeart(time_t dt)
	{
		_dtHeart += dt;
		if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
		{
			CELLLog::Info("checkHeart dead:%d, time=%ld\n", (int)_sockfd, _dtHeart);
			return true;
		}
		return false;
	}
	//定时发送消息时间检测
	bool checkSend(time_t dt)
	{
		_dtSend += dt;
		if (_dtSend >= CLIENT_SEND_BUFF_TIME)
		{
			//CELLLog::Info("checkSend:%d, time=%d\n", _sockfd, _dtSend);
			//定时时间到了立即发送缓冲区中的数据
			SendDataReal();
			//重置发送计时
			resetDTSend();
			return true;
		}
		return false;
	}
private:
	//fdset 文件描述符
	SOCKET _sockfd;
	//第二缓冲区，接受消息缓冲区
	CELLBuffer _recvBuf;
	//发送缓冲区
	CELLBuffer _sendBuf;
	//心跳死亡计时
	time_t _dtHeart;
	//上次发送消息数据的时间
	time_t _dtSend;
	//发送缓冲区写满次数
	int _sendBuffFullCount = 0;
};

#endif // !_CellClient_hpp_
