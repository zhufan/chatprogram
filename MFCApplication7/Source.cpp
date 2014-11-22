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
	addr.sin_port = htons(23444);    //TCP�˿�
	msocket = socket(AF_INET, SOCK_STREAM, 0);
	if (msocket == INVALID_SOCKET)
		AfxMessageBox(_T("TCP����socketʧ��"));
	if (connect(msocket, (const struct sockaddr *)(&addr), sizeof(addr))!=0)
		AfxMessageBox(_T("TCP����ʧ��"));
	this->local = local;
	send(msocket, (const char *)(&local), sizeof(local), 0);
}
TcpProtocol::~TcpProtocol()
{
	if (shutdown(msocket, SD_SEND)==SOCKET_ERROR)   //�ͻ�����߹ر�TCP���ӣ���������������ͣ���ʱ��������recv�����᷵��0.Ȼ�󣬷�����Ҳ�ر����ӣ���߿ͻ��˽����̵߳�recv����Ҳ�᷵��0���൱��ȷ�Ϲرգ�Ȼ������߳��˳���
		AfxMessageBox(_T("TCP���ӿͻ��˹ر�ʧ��"));  
	if (WaitForSingleObject(thread, INFINITE)!=WAIT_OBJECT_0)    //�ȴ������߳��˳�
		AfxMessageBox(_T("TCP���ӷ������ر�ʧ��"));
	CloseHandle(thread);   //�ͷ���Դ
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
		return NULL;            //���recv�����ķ���ֵΪ0��˵�����������Ѿ��رգ��ú�������NULL��Ȼ������߳��յ��������ֵ��ͻ��˳�
	Package *message = new Package(buf);
	return message;   //���ﲻ���ͷ�buf�Ŀռ䣬��Ϊ���ռ��ֱ�message��buf��Ա������
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
		AfxMessageBox(_T("UDP����socketʧ��"));
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23656);   //UDP�����˿�
	sendto(msocket, (const char *)(&local), sizeof(local), 0, (const sockaddr *)(&addr), sizeof(addr));
	recvfrom(msocket, (char *)(&local), sizeof(local), 0, (sockaddr*)(&addr), &addrlen);   //�ͻ��˷���ȷ����Ϣʱ��Ҫ���µ�socket������
	DWORD timeout = 1000;
	if (setsockopt(msocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&timeout), sizeof(timeout))==SOCKET_ERROR)  //����recvfrom����������ʱ��Ϊ1�룬��1���û�յ��ͻ��˵�ȷ��Ӧ����recvfrom��������-1��˵��������
		AfxMessageBox(_T("UDP���ó�ʱʧ��"));
}

UdpProtocol::~UdpProtocol()
{
	sendto(msocket, "", 0, 0, (const sockaddr *)(&addr), addrlen);  //���Ϳհ�
	if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0)    //�ȴ������߳��˳�
		AfxMessageBox(_T("UDP���ӷ������ر�ʧ��"));
	CloseHandle(thread);   //�ͷ���Դ
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
		int result;    //��¼ȷ�Ͻ����1��ʾ�յ���������ȷ��0��ʾ�յ������ݳ���
		if (recvfrom(msocket, (char *)(&result), sizeof(result), 0, (sockaddr*)(&addr), &addrlen) == SOCKET_ERROR)   //˵����ʱ��������
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
		else if (result == SOCKET_ERROR)  //����
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
		swprintf((LPWSTR)tmp, 60,  _T("�ͻ���%d����Ϣ\n"), dia->peer);
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
		notify(_T("�����̳߳ɹ�"));
	else
		notify(_T("�����߳�ʧ��"));
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