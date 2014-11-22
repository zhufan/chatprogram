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
	virtual int mysend(char *data, int len, int peer)=0;   //发送数据
	virtual Package *myrecv()=0;   //接收数据，返回0表示连接已经关闭
	virtual ~Protocol()=0;  //析构函数   
	virtual void setHandle(HANDLE argu) = 0;    //设置接受线程的句柄
};


class TcpProtocol:public Protocol  //协议类，用来实现Tcp协议
{
public:
	void setHandle(HANDLE argu);
	int mysend(char *data, int len, int peer);
	Package *myrecv();  //返回值为NULL说明服务器已经关闭，接收线程需要退出
	TcpProtocol(int local);  //构造函数，用来进行连接
	~TcpProtocol(); //析构函数，用来断开连接
private:
	int msocket;  //通信用socket
	int local;    //本机编号
	HANDLE thread;  //接收线程的句柄
};


class UdpProtocol:public Protocol  //协议类，用来实现Udp协议
{
public:
	void setHandle(HANDLE argu);
	int mysend(char *data, int len, int peer);
	Package *myrecv();     //返回值为NULL说明服务器已经关闭，接收线程需要退出
	UdpProtocol(int local);
	~UdpProtocol();
private:
	int msocket;
	struct sockaddr_in addr;  //目的地
	int addrlen;              //addr长度
	int local;    //本机编号
	HANDLE thread;  //接收线程的句柄
};


class FtpProtocol;  //协议类，用来实现Ftp协议


class RawProtocol;  //协议类，用来实现Ip通信


class Package      //数据类，用来实现数据包以及检错码
{
private:
	char *buf;  //生成的数据包的头，默认的数据包大小为PACKAGE_SIZE，数据包格式：第一个字节为数据包类,接着32位为目标客户端。对于UDP，最后两个字节为检错位，检错位是对整个数据包进行检错
	int type;   //数据包类型，0为客户端上线，1为客户端下线，2为TCP数据，3为UDP数据，4为FTP数据
	int peer;  //peer为上下线的客户端或者是目的客户端
public:
	Package(int type, int peer, char *content); //构造函数，type为类型，peer为对象客户端，content为内容，默认为200字节，用于发送数据时构造数据包
	Package(char *buf);               //构造函数，用于接收时,这里构造时，buf可以直接赋值，不用申请内存后复制
	~Package();
	bool isdestroy();     //Udp判断数据是否被损坏，true为被损坏，false为未被损坏
	char *getcontent();   //获得数据包里的内容，即去掉数据包类型以及检错位，默认大小为200字节，直接赋值即可，不用申请内存
	int getType();
	int getPeer();        //用于返回目的客户端或者是上线或下线的客户端
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
	int getNumber();         //获得当前元素个数
	List();
	~List();
};