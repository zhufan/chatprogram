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
//�췲
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
	virtual int send(char *data, int len)=0;   //��������
	virtual int recv(char *data, int len)=0;   //��������
	virtual ~Protocol()=0;  //��������     
};


class TcpProtocol:public Protocol  //Э���࣬����ʵ��TcpЭ��
{
public:
	int send(char *data, int len);
	int recv(char *data, int len);
	TcpProtocol();  //���캯����������������
	~TcpProtocol(); //���������������Ͽ�����
private:
	int msocket;  //ͨ����socket
};


class UdpProtocol:public Protocol  //Э���࣬����ʵ��UdpЭ��
{
public:
	int send(char *data, int len);
	int recv(char *data, int len);
	UdpProtocol();
	~UdpProtocol();
private:
	int socket;
	struct sockaddr_in addr;  //Ŀ�ĵ�
	int addrlen;              //addr����
};


class FtpProtocol;  //Э���࣬����ʵ��FtpЭ��


class RawProtocol;  //Э���࣬����ʵ��Ipͨ��


class Package      //�����࣬����ʵ�����ݰ��Լ������
{
private:
	char *buf;  //���ɵ����ݰ���ͷ��Ĭ�ϵ����ݰ���СΪPACKAGE_SIZE�����ݰ���ʽ����һ���ֽ�Ϊ���ݰ����͡�����UDP����������ֽ�Ϊ���λ
	int type;   //���ݰ����ͣ�0Ϊ������֪ͨ��1ΪTCP���ݣ�2ΪUDP���ݣ�3ΪFTP����
public:
	Package(int type, char *content); //���캯����typeΪ���ͣ�contentΪ���ݣ�Ĭ��Ϊ200�ֽڣ����ڷ�������ʱ�������ݰ�
	Package(char *buf);               //���캯�������ڽ���ʱ�ƻ����ݰ�
	~Package();
	bool isdestroy();     //Udp�ж������Ƿ��𻵣�trueΪ���𻵣�falseΪδ����
	char *getcontent();   //������ݰ�������ݣ���ȥ�����ݰ������Լ����λ��Ĭ�ϴ�СΪ200�ֽ�
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
	List();
	~List();
};