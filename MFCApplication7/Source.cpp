#include "Header.h"
Dialogue::Dialogue(int peer, int tab)
{
	this->peer = peer;
	this->tab = tab;
}

TcpProtocol::TcpProtocol(int local)
{
	struct sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23444);    //TCP端口
	msocket = socket(AF_INET, SOCK_STREAM, 0);
	if (msocket == INVALID_SOCKET)
		AfxMessageBox(_T("TCP创建socket失败"));
	if (connect(msocket, (const struct sockaddr *)(&addr), sizeof(addr))!=0)
		AfxMessageBox(_T("TCP连接失败"));
	this->local = local;
	send(msocket, (const char *)(&local), sizeof(local), 0);
}
TcpProtocol::~TcpProtocol()
{
	if (shutdown(msocket, SD_SEND)==SOCKET_ERROR)   //客户端这边关闭TCP连接，不再向服务器发送，此时服务器的recv函数会返回0.然后，服务器也关闭连接，这边客户端接收线程的recv函数也会返回0，相当于确认关闭，然后接收线程退出。
		AfxMessageBox(_T("TCP连接客户端关闭失败"));  
	if (WaitForSingleObject(thread, INFINITE)!=WAIT_OBJECT_0)    //等待接收线程退出
		AfxMessageBox(_T("TCP连接服务器关闭失败"));
	CloseHandle(thread);   //释放资源
	closesocket(msocket);
}

int TcpProtocol::mysend(char *data, int len, int peer)
{
	Package *message = new Package(2, peer, data);
	int m = send(msocket, message->getbuf(), PACKAGE_SIZE, 0);
	delete message;
	return m;
}

Package *TcpProtocol::myrecv()
{
	char *buf = new char[PACKAGE_SIZE];
	if (recv(msocket, buf, PACKAGE_SIZE, 0) == 0)
		return NULL;            //如果recv函数的返回值为0，说明服务器端已经关闭，该函数返回NULL，然后接受线程收到这个返回值后就会退出
	Package *message = new Package(buf);
	return message;   //这里不需释放buf的空间，因为这块空间又被message的buf成员引用了
}

void TcpProtocol::setHandle(HANDLE argu)
{
	this->thread = argu;
}

UdpProtocol::UdpProtocol(int local)
{
	this->local = local;
	msocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (msocket==INVALID_SOCKET)
		AfxMessageBox(_T("UDP创建socket失败"));
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23656);   //UDP监听端口
	sendto(msocket, (const char *)(&local), sizeof(local), 0, (const sockaddr *)(&addr), sizeof(addr));
	recvfrom(msocket, (char *)(&local), sizeof(local), 0, (sockaddr*)(&addr), &addrlen);   //客户端发送确认信息时，要用新的socket来发送
	DWORD timeout = 1000;
	if (setsockopt(msocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&timeout), sizeof(timeout))==SOCKET_ERROR)  //设置recvfrom函数的阻塞时间为1秒，若1秒后还没收到客户端的确认应答，则recvfrom函数返回-1，说明丢包了
		AfxMessageBox(_T("UDP设置超时失败"));
}

UdpProtocol::~UdpProtocol()
{
	sendto(msocket, "", 0, 0, (const sockaddr *)(&addr), addrlen);  //发送空包
	if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0)    //等待接收线程退出
		AfxMessageBox(_T("UDP连接服务器关闭失败"));
	CloseHandle(thread);   //释放资源
	closesocket(msocket);
}

void UdpProtocol::setHandle(HANDLE argu)
{
	this->thread = argu;
}

int UdpProtocol::mysend(char *data, int len, int peer)
{
	Package *message = new Package(3, peer, data);
	int m = sendto(msocket, message->getbuf(), PACKAGE_SIZE, 0, (const sockaddr *)(&addr), addrlen);
	while (1)
	{
		int result;    //记录确认结果，1表示收到的数据正确，0表示收到的数据出错
		if (recvfrom(msocket, (char *)(&result), sizeof(result), 0, (sockaddr*)(&addr), &addrlen) == SOCKET_ERROR)   //说明超时，包丢了
			continue;
		else
		{
			if (result)
				break;
			else
				continue;
		}
	}
	delete message;
	return m;
}

Package *UdpProtocol::myrecv()
{
	Package *message;
	while (1)
	{
		char *buf = new char[PACKAGE_SIZE];
		int result = recvfrom(msocket, buf, PACKAGE_SIZE, 0, (sockaddr *)(&addr), &addrlen);
		if (result == 0)
		{
			delete[] buf;
			return NULL;
		}
		else if (result == SOCKET_ERROR)  //丢包
		{
			int back = 0;
			sendto(msocket, (const char *)(&back), sizeof(back), 0, (const sockaddr*)(&addr), addrlen);
			delete[] buf;
			continue;
		}
		else
		{
			message = new Package(buf);
			if (message->isdestroy())
			{
				int back = 0;
				sendto(msocket, (const char *)(&back), sizeof(back), 0, (const sockaddr*)(&addr), addrlen);
				delete message;
				continue;
			}
			else
			{
				int back = 1;
				sendto(msocket, (const char *)(&back), sizeof(back), 0, (const sockaddr*)(&addr), addrlen);
				break;
			}
		}
	}
	return message;
}

Package::Package(int type, int peer, char *content)
{
	buf = new char[PACKAGE_SIZE];
	*buf = type;
	if (type < 2)
	{
		*((int *)(buf + 1)) = *((int *)content);
	}
	else if (type == 2)
	{
		*((int *)(buf + 1)) = peer;
		for (int i = 0; i < 200; i++)
			*(buf + 1 + sizeof(int)+i) = *(content + i);
	}
	else if (type == 3)
	{
		*((int *)(buf + 1)) = peer;
		for (int i = 0; i < 200; i++)
			*(buf + 1 + sizeof(int) + i) = *(content + i);
		*((short *)(buf + PACKAGE_SIZE - 2)) = 0;
		for (int i = 0; i < PACKAGE_SIZE / 2 - 1; i++)
			*((short *)(buf + PACKAGE_SIZE - 2)) += *(((short *)(buf)+i));
	}
	this->type = type;
	this->peer = peer;
}

Package::Package(char *buf)
{
	this->buf = buf;
	type = *buf;
	peer = *((int *)(buf + 1));
}

Package::~Package()
{
	delete[] buf;
}

bool Package::isdestroy()
{
	short result = 0;
	for (int i = 0; i < PACKAGE_SIZE / 2 - 1; i++)
		result += *(((short *)(buf)+i));
	if (result == *((short *)(buf + PACKAGE_SIZE - 2)))
		return false;
	else
		return true;
}

char *Package::getcontent()
{
	return buf + 1 + sizeof(int);
}

int Package::getType()
{
	return type;
}

int Package::getPeer()
{
	return peer;
}

char *Package::getbuf()
{
	return buf;
}

void CMFCApplication7Dlg::notify(LPCTSTR info)
{
	infoBox.SetSel(1000, 1000);
	infoBox.ReplaceSel(info);
	inputBox.SetFocus();
}

void CMFCApplication7Dlg::showContent(Dialogue *dia, LPCTSTR content)
{
	dia->content += content;
	if (dia = currentDialogue)
	{
		contentBox.SetSel(1000, 1000);
		contentBox.ReplaceSel(content);
		inputBox.SetFocus();
	}
	else
	{
		wchar_t *tmp = new wchar_t[200];
		swprintf((LPWSTR)tmp, 60,  _T("客户端%d来信息\n"), dia->peer);
		notify((LPCTSTR)tmp);
		delete[] tmp;
	}
}

void CMFCApplication7Dlg::sendmessage()
{
	CString tmp;
	inputBox.GetWindowTextW(tmp);
	tmp += _T("\n");
	inputBox.SetSel(0, -1);
	inputBox.Clear();
	currentProtocol->mysend((char *)(tmp.GetBuffer()), 200, currentDialogue->peer);
	showContent(currentDialogue, (LPCTSTR)(tmp.GetBuffer()));
	inputBox.SetFocus();
}

void CMFCApplication7Dlg::changeProtocol()
{
	mTab.DeleteAllItems();
	if (dialogueList)
	    delete dialogueList;
	dialogueList = new List();
	currentDialogue = NULL;
	if (currentProtocol)
		delete currentProtocol;
	CString proto1 = _T("TCP");
	CString proto2 = _T("UDP");
	CString proto;
	protocolBox.GetWindowTextW(proto);
	if (proto == proto1)
		currentProtocol = new TcpProtocol(local);
	else if (proto = proto2)
		currentProtocol = new UdpProtocol(local);
	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvfuc, (LPVOID)this, 0, NULL);
	if (thread)
		notify(_T("创建线程成功"));
	else
		notify(_T("创建线程失败"));
	currentProtocol->setHandle(thread);
}

void CMFCApplication7Dlg::changeDialogue(int i)
{
	currentDialogue = dialogueList->getElem(i);
	mTab.SetCurSel(i);
	wchar_t *tmp = new wchar_t[60];
	swprintf(tmp, 50, _T("%d"), currentDialogue->peer);
	purposeBox.SetWindowTextW(tmp);
	delete tmp;
	contentBox.SetSel(0, -1);
	contentBox.Clear();
	contentBox.SetWindowTextW(currentDialogue->content.GetBuffer());
	contentBox.SetSel(1000, 1000);
	inputBox.SetFocus();
	RedrawWindow();
}

void CMFCApplication7Dlg::addDialogue(int peer)
{
	int tab = dialogueList->getNumber();
	Dialogue *dia = new Dialogue(peer, tab);
	dialogueList->insert(dia);
	wchar_t *tmp = new wchar_t[60];
	swprintf(tmp, 50, _T("%d"), peer);
	purposeBox.AddString(tmp);
	delete tmp;
}