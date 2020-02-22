#ifndef I_NET_ENENT_HPP
#define I_NET_ENENT_HPP

#include "CELL.hpp"
#include "CELLClient.hpp"
//自定义
class CellServer;

//网络事件接口
class INetEvent
{
public:
	//纯虚函数
	//客户端加入事件
	virtual void OnNetJoin(CellClient* pClient) = 0;
	//客户端离开事件
	virtual void OnNetLeave(CellClient* pClient) = 0;
	//客户端消息事件
	virtual void onNetMsg(CellServer* pCellServer, CellClient* pClient, DataHeader* header) = 0;
	//客户端接受事件
	virtual void onNetRecv(CellClient* pClient) = 0;
private:

};

#endif // !I_NET_ENENT_HPP
