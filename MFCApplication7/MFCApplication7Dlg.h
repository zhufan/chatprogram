
// MFCApplication7Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Header.h"

// CMFCApplication7Dlg 对话框
class CMFCApplication7Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication7Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCAPPLICATION7_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
//public:
	//afx_msg void OnBnClickedOk();
private:
	CEdit contentBox;    //内容显示框
	CTabCtrl mTab;       //标签页
	CEdit infoBox;       //系统信息页
	CEdit inputBox;      //输入框
	CEdit localBox;      //本机编号输入框
	CComboBox protocolBox;  //协议复选框
	CComboBox purposeBox;  //目的客户端复选框
	Protocol *currentProtocol;  //当前通信使用的协议
	bool isInitialize;        //表示相关的网络服务是否初始化，true表示已经初始化，初始值为false
	bool isConnected;         //表示当前协议下是否与服务器连接，初始值为false
	List *dialogueList;        //会话列表
	Dialogue *currentDialogue;  //当前会话
	int local;                //本机编号
public:
	afx_msg void OnBnClickedButton();
	afx_msg void OnClickedQuitButton();
	afx_msg void OnClickedLinkButton();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	void notify(LPCTSTR info);     //显示系统通知，换行符要自己加
	void sendmessage();        //发送信息
	void changeProtocol();    //切换协议
	void changeDialogue(int i);  //切换会话
	void showContent(Dialogue *dia, LPCTSTR content);  //在某个会话的内容显示框内显示内容
	void addDialogue(int peer);    //增加一个会话，参数为对方客户端的编号
	void deleteDialogue(int peer);  //删除一个会话，参数为对方客户端的编号
	static DWORD recvfuc(LPVOID argu);  //接收线程函数
};
