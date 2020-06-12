// chart.cpp : implementation file
//
#include "pch.h"
#include "stdafx.h"
#include "CPUUsage.h"
#include "chart.h"
#include "afxdialogex.h"


// chart dialog

IMPLEMENT_DYNAMIC(chart, CDialog)

chart::chart(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_chart, pParent)
{
}

chart::~chart()
{
}

void chart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_CHART, m_ChartCtrl);
}

BEGIN_MESSAGE_MAP(chart, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

#define DATA_SHOW_LENGHT 50 //总共显示的点个数
#define DATA_UPDATE_LENGHT 1 //每次更新的点个数
#define DATA_SHOW_X_AXIS 50 //X轴显示的点最大值
#define DATA_SHOW_Y_AXIS 100 //Y轴显示的点最大值
//要显示点的缓冲数据
static double xBuff[DATA_SHOW_LENGHT + 1] = { 0 };
static double yBuff[DATA_SHOW_LENGHT + 1] = { 0 };

//显示点数据包初始化
void chart::DataBuffInit(void)
{
	for (int i = 0; i < DATA_SHOW_LENGHT + 1; i++) {
		xBuff[i] = i;
		yBuff[i] = 5;// cos((i)) * 10 + 50;
	}
}

//初始化画图界面窗口
void chart::ChartCtrlInit(void) {
	m_ChartCtrl.SetBackColor(RGB(0, 255, 0));
	///////////////////////显示主题/////////////////////////////
	m_ChartCtrl.GetTitle()->AddString(_T("CPU Usage: %"));
	///////////////////////创建坐标显示范围/////////////////////////////
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetMinMax(0, DATA_SHOW_X_AXIS);
	//pAxis->GetLabel()->SetText(_T("Time"));
	pAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetMinMax(0, DATA_SHOW_Y_AXIS);
	//pAxis->GetLabel()->SetText(_T("Usage"));
}


// CmyApplicationDlg 消息处理程序
BOOL chart::OnInitDialog()
{
	CDialog::OnInitDialog();
	//获取显示的对话框大小
	CRect rect;
	GetClientRect(&rect);
	oldPiont.x = rect.right - rect.left;
	oldPiont.y = rect.bottom - rect.top;
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作

	// TODO: 在此添加额外的初始化代码
	//ModifyStyle(WS_CAPTION, NULL, SWP_DRAWFRAME);
	DataBuffInit();
	ChartCtrlInit();

	if (!cpu1.Initialize())
	{
		printf("initial fail");
		return -1;
	}
	else {
		SetTimer(0, 1000, NULL); //设置一秒刷新一次
	}	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void chart::OnPaint()
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

	}
	else
	{
		CDialog::OnPaint();
	}
}

void chart::Resize(void) {
	float fsp[2];
	POINT newPoint;//获取当前对话框大小
	CRect newRect;//获取当前对话框的坐标
	GetClientRect(&newRect);
	newPoint.x = newRect.right - newRect.left;
	newPoint.y = newRect.bottom - newRect.top;
	fsp[0] = (float)newPoint.x / oldPiont.x;
	fsp[1] = (float)newPoint.y / oldPiont.y;

	int woc;
	CRect rect;
	CPoint oldTLPoint, newTLPoint;//左上角
	CPoint oldBRPoint, newBRPoint;//右下角
								  //列出所有的子空间
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild) {
		woc = ::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(woc)->GetWindowRect(rect);
		ScreenToClient(rect);

		oldTLPoint = rect.TopLeft();
		newTLPoint.x = long(oldTLPoint.x*fsp[0]);
		newTLPoint.y = long(oldTLPoint.y*fsp[1]);
		oldBRPoint = rect.BottomRight();
		newBRPoint.x = long(oldBRPoint.x*fsp[0]);
		newBRPoint.y = long(oldBRPoint.y*fsp[1]);

		rect.SetRect(newTLPoint, newBRPoint);
		GetDlgItem(woc)->MoveWindow(rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	oldPiont = newPoint;
	return;
}
void chart::OnSize(UINT nType, int cx, int cy) {
	//窗体大小发生变动。处理函数resize
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		Resize();
	}
}
void chart::DataShow(double *xb, double *yb, int len) {
	m_ChartCtrl.EnableRefresh(false);
	CChartLineSerie *pLineSerie;
	m_ChartCtrl.RemoveAllSeries();
	pLineSerie = m_ChartCtrl.CreateLineSerie();
	pLineSerie->SetSeriesOrdering(poNoOrdering);//设置为无序
	pLineSerie->AddPoints(xb, yb, len);
	UpdateWindow();
	m_ChartCtrl.EnableRefresh(true);
}
void chart::OnTimer(UINT nIDEvent) {
	//static int offset = 0;
	for (int m = 0; m < DATA_SHOW_LENGHT - DATA_UPDATE_LENGHT + 1; m++)
	{
		//xd[m] = xd[DATA_UPDATE_LENGHT + m];
		yBuff[m] = yBuff[DATA_UPDATE_LENGHT + m];
	}
	int index = DATA_SHOW_LENGHT - DATA_UPDATE_LENGHT + 1;
	for (int i = 0; i < DATA_UPDATE_LENGHT; i++)
	{
		int rate = cpu1.GetCPUUseRate();
		yBuff[index + i] = rate;
		m_ChartCtrl.GetTitle()->RemoveAll();
		TCHAR Temp[64] = TEXT("");
		_stprintf_s(Temp, TEXT("CPU Usage: %2d%%"), rate);
		m_ChartCtrl.GetTitle()->AddString(Temp);

		//yd[index + i] = cos((index + i + w)/5) * 50 + 100+rand() / 1000;
		//yBuff[index + i] = cos((i + offset) / 5) * DATA_SHOW_Y_AXIS / 4 + DATA_SHOW_Y_AXIS / 2;
	}
	DataShow(xBuff, yBuff, DATA_SHOW_LENGHT + 1);
	//offset++;
	//if (offset > 10000) {
	//	offset = 0;
	//}
}
