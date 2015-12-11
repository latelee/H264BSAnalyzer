#pragma once

class CH264BSAnalyzerDlg;

// CPlayDlg dialog

class CPlayDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayDlg)

public:
	CPlayDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayDlg();

// Dialog Data
	enum { IDD = IDD_PLAYDLG };

public:
    void SetParentWnd(CH264BSAnalyzerDlg* pWnd) {m_pParentWnd = pWnd;}
private:
    CH264BSAnalyzerDlg *m_pParentWnd; // 窗口参数传递

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
