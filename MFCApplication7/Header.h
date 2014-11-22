#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "MFCApplication7.h"
#include "MFCApplication7Dlg.h"
#include "afxdialogex.h"
#define PACKAGE_SIZE 220
//朱凡
class Dialogue     //对话类，用于保存两个客户端的对话内容以及记录对话对象
{
public:
	CString content;   //用来保存对话显示框中的类容，便于进行会话标签页的切换
	int peer;          //对方客户端的编号
	int tab;           //本会话的标签页的编号（从0开始）
	Dialogue(int peer, int tab);
};


class Protocol     //协议类，抽象类，用来作为TcpProtocol,UdpProtocol等协议类的接口，实现多态
{
public:
	virtual int send(char *data, int len)=0;   //发送数据
	virtual int recv(char *data, int len)=0;   //接收数据
	virtual ~Protocol()=0;  //析构函数     
};


class TcpProtocol:public Protocol  //协议类，用来实现Tcp协议
{
public:
	int send(char *data, int len);
	int recv(char *data, int len);
	TcpProtocol();  //构造函数，用来进行连接
	~TcpProtocol(); //析构函数，用来断开连接
private:
	int msocket;  //通信用socket
};


class UdpProtocol:public Protocol  //协议类，用来实现Udp协议
{
public:
	int send(char *data, int len);
	int recv(char *data, int len);
	UdpProtocol();
	~UdpProtocol();
private:
	int socket;
	struct sockaddr_in addr;  //目的地
	int addrlen;              //addr长度
};


class FtpProtocol;  //协议类，用来实现Ftp协议


class RawProtocol;  //协议类，用来实现Ip通信


class Package      //数据类，用来实现数据包以及检错码
{
private:
	char *buf;  //生成的数据包的头，默认的数据包大小为PACKAGE_SIZE，数据包格式：第一个字节为数据包类型。对于UDP，最后两个字节为检错位
	int type;   //数据包类型，0为服务器通知，1为TCP数据，2为UDP数据，3为FTP数据
public:
	Package(int type, char *content); //构造函数，type为类型，content为内容，默认为200字节，用于发送数据时构造数据包
	Package(char *buf);               //构造函数，用于接收时破坏数据包
	~Package();
	bool isdestroy();     //Udp判断数据是否被损坏，true为被损坏，false为未被损坏
	char *getcontent();   //获得数据包里的内容，即去掉数据包类型以及检错位，默认大小为200字节
	char *getbuf();       //获得数据包，用于发送数据包
};

class List   //列表数据结构，用于会话列表
{
private:
	Dialogue **head;    //列表头
	int bigestSize;     //列表的最大容量
	int number;         //当前元素个数
public:
	Dialogue *getElem(int i);  //获得第i个元素的引用，从0开始
	void insert(Dialogue *elem);  //插入一个元素
	void remove(int i);       //删除一个元素
	List();
	~List();
};