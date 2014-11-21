#include "Header.h"
Dialogue::Dialogue(int peer, int tab)
{
	this->peer = peer;
	this->tab = tab;
}

TcpProtocol::TcpProtocol()
{
	struct sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23444);    //TCP¶Ë¿Ú
	msocket = socket(AF_INET, SOCK_STREAM, 0);
	if (msocket == INVALID_SOCKET)
		AfxMessageBox(_T("TCP´´½¨socketÊ§°Ü"));
	if (connect(msocket, ))
}