#ifndef _CELL_BUFFER_HPP_
#define _CELL_BUFFER_HPP_
#include "CELL.hpp"
class CELLBuffer
{
public:
	CELLBuffer(int nSize = 8192)
	{
		_nSize = nSize;
		_pBuf = new char[_nSize];
	}
	~CELLBuffer()
	{
		if (_pBuf)
		{
			delete[] _pBuf;
			_pBuf = nullptr;
		}
	}

	char* data()
	{
		return _pBuf;
	}
	bool push(const char* pData, int nLen)
	{
		//if (_nLast + nLen > _nSize)
		//{
		//	//写入大量数据不一定要放到内存中
		//	//也可以存储到数据库或磁盘
		//	//需要写入的数据大于可用空间
		//	int newSize = (_nLast + nLen) - _nSize;
		//	//拓展BUFF大小
		//	if (newSize < 8192)
		//		newSize = 8192;
		//	char* buf = new char[newSize];
		//	memcpy(buf, _pBuf, _nLast);
		//	delete[] _pBuf;
		//	_pBuf = buf;
		//}

		if (_nLast + nLen <= _nSize) {
			//将要发送的数据，拷贝到发送缓冲区尾部
			memcpy(_pBuf + _nLast, pData, nLen);
			//计算数据尾部位置
			_nLast += nLen;
			if (_nLast == SEND_BUFF_SIZE)
			{
				_fullCount++;
			}

			return true;
		}
		else {
			_fullCount++;
		}
		return false;
	}

	void pop(int nLen)
	{
		int n = _nLast - nLen;
		if (n > 0)
		{
			memcpy(_pBuf, _pBuf + nLen, n);
		}
		_nLast = n;
		if (_fullCount > 0)
			--_fullCount;
	}

	int write2socket(SOCKET sockfd)
	{
		int ret = 0;
		//缓冲区有数据
		if (_nLast > 0 && INVALID_SOCKET != sockfd)
		{
			//发送数据
			ret = send(sockfd, _pBuf, _nLast, 0);
			//数据尾部位置清零
			_nLast = 0;
			//重置定时发送计时
			//
			_fullCount = 0;
			//
		}
		return ret;
	}

	int read4socket(SOCKET sockfd)
	{
		if (_nSize - _nLast > 0)
		{
			//接收客户端数据
			char* szRecv = _pBuf + _nLast;
			int nLen = (int)recv(sockfd, szRecv, _nSize - _nLast, 0);
			//CELLLog::Info("nLen=%d\n", nLen);
			if (nLen <= 0)
			{
				return nLen;
			}
			_nLast += nLen;
			return nLen;
		}

		return 0;
	}

	bool hasMsg()
	{
		//判读消息缓冲区的长度大于消息头DataHeader长度
		if (_nLast >= sizeof(DataHeader))
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)_pBuf;
			//判断消磁缓冲区的长度大于消息长度
			return _nLast >= header->dataLength;
		}
		return false;
	}
private:
	//第二缓冲区，发送缓冲区

	//可以用链表和队列来管理缓冲数据块
	//list<char*> _pBuffList;

	char* _pBuf = nullptr;
	//缓冲区的数据尾部位置，已有数据长度
	int _nLast = 0;
	//缓冲区总的空间大小
	int _nSize = 0;
	//缓冲区写满次数计数
	int _fullCount = 0;
};

#endif