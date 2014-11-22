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
class Dialogue     //�Ի��࣬���ڱ��������ͻ��˵ĶԻ������Լ���¼�Ի�����
{
public:
	CString content;   //��������Ի���ʾ���е����ݣ����ڽ��лỰ��ǩҳ���л�
	int peer;          //�Է��ͻ��˵ı��
	int tab;           //���Ự�ı�ǩҳ�ı�ţ���0��ʼ��
	Dialogue(int peer, int tab);
};


class Protocol     //Э���࣬�����࣬������ΪTcpProtocol,UdpProtocol��Э����Ľӿڣ�ʵ�ֶ�̬
{
public:
	virtual int mysend(char *data, int len, int peer)=0;   //��������
	virtual Package *myrecv()=0;   //�������ݣ�����0��ʾ�����Ѿ��ر�
	virtual ~Protocol()=0;  //��������   
	virtual void setHandle(HANDLE argu) = 0;    //���ý����̵߳ľ��
};


class TcpProtocol:public Protocol  //Э���࣬����ʵ��TcpЭ��
{
public:
	void setHandle(HANDLE argu);
	int mysend(char *data, int len, int peer);
	Package *myrecv();  //����ֵΪNULL˵���������Ѿ��رգ������߳���Ҫ�˳�
	TcpProtocol(int local);  //���캯����������������
	~TcpProtocol(); //���������������Ͽ�����
private:
	int msocket;  //ͨ����socket
	int local;    //�������
	HANDLE thread;  //�����̵߳ľ��
};


class UdpProtocol:public Protocol  //Э���࣬����ʵ��UdpЭ��
{
public:
	void setHandle(HANDLE argu);
	int mysend(char *data, int len, int peer);
	Package *myrecv();     //����ֵΪNULL˵���������Ѿ��رգ������߳���Ҫ�˳�
	UdpProtocol(int local);
	~UdpProtocol();
private:
	int msocket;
	struct sockaddr_in addr;  //Ŀ�ĵ�
	int addrlen;              //addr����
	int local;    //�������
	HANDLE thread;  //�����̵߳ľ��
};


class FtpProtocol;  //Э���࣬����ʵ��FtpЭ��


class RawProtocol;  //Э���࣬����ʵ��Ipͨ��


class Package      //�����࣬����ʵ�����ݰ��Լ������
{
private:
	char *buf;  //���ɵ����ݰ���ͷ��Ĭ�ϵ����ݰ���СΪPACKAGE_SIZE�����ݰ���ʽ����һ���ֽ�Ϊ���ݰ���,����32λΪĿ��ͻ��ˡ�����UDP����������ֽ�Ϊ���λ�����λ�Ƕ��������ݰ����м��
	int type;   //���ݰ����ͣ�0Ϊ�ͻ������ߣ�1Ϊ�ͻ������ߣ�2ΪTCP���ݣ�3ΪUDP���ݣ�4ΪFTP����
	int peer;  //peerΪ�����ߵĿͻ��˻�����Ŀ�Ŀͻ���
public:
	Package(int type, int peer, char *content); //���캯����typeΪ���ͣ�peerΪ����ͻ��ˣ�contentΪ���ݣ�Ĭ��Ϊ200�ֽڣ����ڷ�������ʱ�������ݰ�
	Package(char *buf);               //���캯�������ڽ���ʱ,���ﹹ��ʱ��buf����ֱ�Ӹ�ֵ�����������ڴ����
	~Package();
	bool isdestroy();     //Udp�ж������Ƿ��𻵣�trueΪ���𻵣�falseΪδ����
	char *getcontent();   //������ݰ�������ݣ���ȥ�����ݰ������Լ����λ��Ĭ�ϴ�СΪ200�ֽڣ�ֱ�Ӹ�ֵ���ɣ����������ڴ�
	int getType();
	int getPeer();        //���ڷ���Ŀ�Ŀͻ��˻��������߻����ߵĿͻ���
	char *getbuf();       //������ݰ������ڷ������ݰ�
};

class List   //�б����ݽṹ�����ڻỰ�б�
{
private:
	Dialogue **head;    //�б�ͷ
	int bigestSize;     //�б���������
	int number;         //��ǰԪ�ظ���
public:
	Dialogue *getElem(int i);  //��õ�i��Ԫ�ص����ã���0��ʼ
	void insert(Dialogue *elem);  //����һ��Ԫ��
	void remove(int i);       //ɾ��һ��Ԫ��
	int getNumber();         //��õ�ǰԪ�ظ���
	List();
	~List();
};