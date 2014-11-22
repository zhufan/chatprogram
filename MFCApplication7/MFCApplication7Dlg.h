
// MFCApplication7Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Header.h"

// CMFCApplication7Dlg �Ի���
class CMFCApplication7Dlg : public CDialogEx
{
// ����
public:
	CMFCApplication7Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCAPPLICATION7_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
//public:
	//afx_msg void OnBnClickedOk();
private:
	CEdit contentBox;    //������ʾ��
	CTabCtrl mTab;       //��ǩҳ
	CEdit infoBox;       //ϵͳ��Ϣҳ
	CEdit inputBox;      //�����
	CEdit localBox;      //������������
	CComboBox protocolBox;  //Э�鸴ѡ��
	CComboBox purposeBox;  //Ŀ�Ŀͻ��˸�ѡ��
	Protocol *currentProtocol;  //��ǰͨ��ʹ�õ�Э��
	bool isInitialize;        //��ʾ��ص���������Ƿ��ʼ����true��ʾ�Ѿ���ʼ������ʼֵΪfalse
	bool isConnected;         //��ʾ��ǰЭ�����Ƿ�����������ӣ���ʼֵΪfalse
	List *dialogueList;        //�Ự�б�
	Dialogue *currentDialogue;  //��ǰ�Ự
	int local;                //�������
public:
	afx_msg void OnBnClickedButton();
	afx_msg void OnClickedQuitButton();
	afx_msg void OnClickedLinkButton();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	void notify(LPCTSTR info);     //��ʾϵͳ֪ͨ�����з�Ҫ�Լ���
	void sendmessage();        //������Ϣ
	void changeProtocol();    //�л�Э��
	void changeDialogue(int i);  //�л��Ự
	void showContent(Dialogue *dia, LPCTSTR content);  //��ĳ���Ự��������ʾ������ʾ����
	void addDialogue(int peer);    //����һ���Ự������Ϊ�Է��ͻ��˵ı��
	void deleteDialogue(int peer);  //ɾ��һ���Ự������Ϊ�Է��ͻ��˵ı��
	static DWORD recvfuc(LPVOID argu);  //�����̺߳���
};
