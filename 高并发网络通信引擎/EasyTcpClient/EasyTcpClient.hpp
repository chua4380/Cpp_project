#ifndef _EasyTcpClient_hpp_
#define __EasyTcpClient_hpp_

#include "CELL.hpp"
#include "MessageHeader.hpp"

class EasyTcpClient
{
	SOCKET _sock;
	bool _isConnect;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
		_isConnect = false;
	}
	virtual ~EasyTcpClient()
	{
		Close();
	}
	//初始化socket
	void InitSocket()
	{
#ifdef _WIN32
		//启动Win Sock 2.x环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		
		if (INVALID_SOCKET != _sock)
		{
			printf("<sock=%d>关闭了之前的连接...\n", (int)_sock);
			Close();
		}
		// 1.建立一个socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		
		if (INVALID_SOCKET == _sock) {
			printf("错误，建立socket失败...\n");
		}
		else {
			//printf("建立socket成功...\n");
		}
	}
	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		// 2.连接服务器 connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret) {
			printf("错误，连接服务器失败...\n");
		}
		else {
			//printf("连接服务器成功...\n");
			_isConnect = true;
		}
		return ret;
	}
	//关闭socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			// 清除windows socket环境
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
		_isConnect = false;
	}
	//查询网络消息
	int _nCount = 0;
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 0,0 };
			int ret = select((int)_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket=%d>select任务结束1\n", (int)_sock);
				Close();
				return false;
			}
			if (FD_ISSET(_sock, &fdReads)) {
				FD_CLR(_sock, &fdReads);

				if (-1 == RecvData(_sock)) {
					printf("<socket=%d>select任务结束2\n", (int)_sock);
					Close();
					return false;
				}
			}
		}
		return true;
	
	}
	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET && _isConnect;
	}
	//缓冲区最小单元大小
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif
	//接受缓冲区
	//char _szRecv[RECV_BUFF_SIZE] = {};
	//第二缓冲区 消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE] = {};
	//消息缓冲区数据尾部位置
	int _lastPos = 0;
	//接受数据 处理粘包 拆分包
	int RecvData(SOCKET cSock)
	{
		
		// 5.接收数据
		char* szRecv = _szMsgBuf + _lastPos;
		int nLen = (int)recv(cSock, szRecv, (RECV_BUFF_SIZE) - _lastPos, 0);

		if (nLen <= 0)
		{
			printf("与服务器断开连接,任务结束。\n");
			return -1;
		}
		//将收取到的数据拷贝到消息缓冲区
		//memcpy(_szMsgBuf+_lastPos, _szRecv, nLen);
		//消息缓冲区的数据尾部位置后移
		_lastPos += nLen;
		//判读消息缓冲区的长度大于消息头DataHeader长度
		while (_lastPos >= sizeof(DataHeader))
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)_szMsgBuf;
			//判断消磁缓冲区的长度大于消息长度
			if (_lastPos >= header->dataLength)
			{
				//消息缓冲区剩余未处理数据的长度
				int nSize = _lastPos - header->dataLength;
				//处理网络消息
				OnNetMsg(header);
				//将消息缓冲区剩余未处理数据前移
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, _lastPos - header->dataLength);
				//将消息缓冲区的数据尾部位置前移
				_lastPos = nSize;
			}
			else {
				//消息缓冲区数据不够一条完整消息
				break;
			}
		}
		return 0;
	}
	//响应网络消息
	virtual void OnNetMsg(DataHeader* header)
	{
		switch (header->cmd) {
			case CMD_LOGIN_RESULT:
			{
				netmsg_LoginResult* login = (netmsg_LoginResult*)header;
				//printf("<Socket=%d>收到服务器消息：CMD_LOGIN_RESULT, 数据长度 : %d\n", (int)_sock, header->dataLength);
			}
			break;
			case CMD_LOGOUT_RESULT:
			{
				netmsg_LogoutResult* logout = (netmsg_LogoutResult*)header;
				//printf("<Socket=%d>收到服务器消息：CMD_LOGOUT_RESULT, 数据长度 : %d\n", (int)_sock, header->dataLength);
			}
			break;
			case CMD_NEW_USER_JOIN:
			{
				NewUserJoin* userJoin = (NewUserJoin*)header;
				//printf("<Socket=%d>收到服务器消息：CMD_NEW_USER_JOIN, 数据长度 : %d\n", (int)_sock, header->dataLength);
			}
			break;
			case CMD_ERROR:
			{
				printf("<Socket=%d>收到服务器消息：CMD_ERROR, 数据长度 : %d\n", (int)_sock, header->dataLength);
			}
			break;
			default:
			{
				printf("<Socket=%d>收到未定义消息，数据长度 : %d\n", (int)_sock, header->dataLength);
			}
		}
	}
	//发送数据
	int SendData(DataHeader* header, int nLen)
	{
		int ret = SOCKET_ERROR;
		if (isRun() && header)
		{
			ret = send(_sock, (const char*)header, nLen, 0);
			if (SOCKET_ERROR == ret)
			{
				Close();
			}

		}
		return ret;
	}
private:

};


#endif