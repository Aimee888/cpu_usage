#pragma once
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxisLabel.h"

// chart dialog

class chart : public  CDialog
{
	DECLARE_DYNAMIC(chart)

public:
	chart(CWnd* pParent = nullptr);   // standard constructor
	virtual ~chart();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_chart };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		// 实现
protected:

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	POINT oldPiont;
	void Resize(void);
	CChartCtrl m_ChartCtrl;
	void ChartCtrlInit(void);
	void DataBuffInit(void);
	void DataShow(double *xb, double *yb, int len);
};
