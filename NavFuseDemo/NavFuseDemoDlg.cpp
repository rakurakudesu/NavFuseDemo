
// NavFuseDemoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "NavFuseDemo.h"
#include "NavFuseDemoDlg.h"
#include "afxdialogex.h"
#include "CNavFuseDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNavFuseDemoDlg 对话框

CNavFuseDemoDlg::CNavFuseDemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NAVFUSEDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNavFuseDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNavFuseDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CNavFuseDemoDlg 消息处理程序

BOOL CNavFuseDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 1. 获取IDC_DRAW控件的位置（作为视图的大小和坐标）
	UINT ID = IDC_DRAW;
	CWnd* pWnd = this->GetDlgItem(ID);  // 获取静态文本控件的窗口指针
	if (pWnd == NULL) return FALSE;     // 容错：若控件不存在，初始化失败

	CRect Rect;
	pWnd->GetWindowRect(Rect);          // 获取控件在屏幕上的坐标
	this->ScreenToClient(Rect);         // 转换为对话框的客户区坐标（关键：避免位置偏移）

	// 2. 动态创建CNavFuseDemoView实例
	CNavFuseDemoView* m_pview = (CNavFuseDemoView*)RUNTIME_CLASS(CNavFuseDemoView)->CreateObject();
	if (NULL == m_pview) return FALSE;  // 容错：若创建失败，初始化失败

	// 3. 创建视图窗口（嵌入到对话框中）
	// 补充WS_CHILD（子窗口）和WS_VISIBLE（创建后显示），否则视图不显示
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW | WS_CHILD | WS_VISIBLE;
	BOOL bCreate = m_pview->Create(
		NULL,               // 类名：使用默认视图类名
		NULL,               // 窗口标题：无需标题
		dwStyle,            // 窗口样式（关键：子窗口+可见）
		Rect,               // 视图的位置和大小（来自IDC_DRAW）
		this,               // 父窗口：对话框本身
		ID                  // 视图的ID（与容器控件ID一致，避免冲突）
	);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNavFuseDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNavFuseDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNavFuseDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

