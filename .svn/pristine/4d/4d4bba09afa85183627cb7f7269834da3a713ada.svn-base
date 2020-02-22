#ifndef _CELL_NETWORK_HPP_
#define _CELL_NETWORK_HPP_

#include "CELL.hpp"

class CELLNetwork
{
private:
	CELLNetwork()
	{
#ifdef _WIN32
		//启动Win Sock 2.x环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
#ifndef _WIN32
		//if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		//	return  (1);
		//忽略异常信号，默认情况会导致进程终止
		signal(SIGPIPE, SIG_IGN);
#endif
	}
	~CELLNetwork()
	{
#ifdef _WIN32
		// 清除windows socket环境
		WSACleanup();
#endif
	}
public:
	static void Init() {
		static CELLNetwork obj;
	}
};

#endif // !_CELL_NETWORK_HPP_
