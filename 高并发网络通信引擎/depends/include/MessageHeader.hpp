#ifndef _MESSAGE_HEADER_HPP_
#define _MESSAGE_HEADER_HPP_
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_C2S_HEART,
	CMD_S2C_HEART,
	CMD_ERROR
};
struct DataHeader
{
	DataHeader()
	{
		dataLength = sizeof(DataHeader);
		cmd = CMD_ERROR;
	}
	short dataLength;
	short cmd;
};
//DataPackage
struct netmsg_Login : public DataHeader
{
	netmsg_Login() {
		dataLength = sizeof(netmsg_Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
	char data[32];
};
struct netmsg_LoginResult : public DataHeader {
	netmsg_LoginResult() {
		dataLength = sizeof(netmsg_LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
	char data[92];
};
struct netmsg_Logout : public DataHeader
{
	netmsg_Logout() {
		dataLength = sizeof(netmsg_Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};
struct netmsg_LogoutResult : public DataHeader
{
	netmsg_LogoutResult() {
		dataLength = sizeof(netmsg_LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUserJoin : public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		scok = 0;
	}
	int scok;
};

struct netmsg_s2c_Heart : public DataHeader
{
	netmsg_s2c_Heart() {
		dataLength = sizeof(netmsg_s2c_Heart);
		cmd = CMD_S2C_HEART;
	}
};

struct netmsg_c2s_Heart : public DataHeader
{
	netmsg_c2s_Heart() {
		dataLength = sizeof(netmsg_c2s_Heart);
		cmd = CMD_C2S_HEART;
	}
};
#endif // !_MESSAGE_HEADER_HPP_
