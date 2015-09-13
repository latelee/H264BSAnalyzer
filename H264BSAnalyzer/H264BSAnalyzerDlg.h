
// H264BSAnalyzerDlg.h : header file
//
#include "resource.h"
#include "stdafx.h"

#include "NalParse.h"
#include "hexeditctrl.h"

#include <vector>
using std::vector;

#pragma once

#define MAX_URL_LENGTH 512

// CH264BSAnalyzerDlg dialog
class CH264BSAnalyzerDlg : public CDialogEx
{
// Construction
public:
    CH264BSAnalyzerDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_H264BSANALYZER_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // our own...
public:
    void SystemClear();

    int ShowNLInfo(NALU_t* nalu);
// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    //CMFCEditBrowseCtrl m_h264InputUrl;  // 保留用
    CEdit m_h264NalInfo;
    CListCtrl m_h264NalList;
    CEdit m_edFileUrl;
    CHexEdit m_edHexInfo;   // 显示十六进制
    CComboBox m_cbNalNum;   // 分析NAL数量

    afx_msg void OnBnClickedH264InputurlOpen();

    // our own
private:
    int m_nSliceIndex;
    CString m_strFileUrl;
    char str_szFileUrl[MAX_URL_LENGTH];

    vector<NALU_t> m_vNalTypeVector;
    int m_nValTotalNum; // m_vNalTypeVector有多少个NALU_t

    HANDLE m_hFileThread;
    HANDLE m_hNALThread;

    HANDLE m_hFileLock;
    HANDLE m_hNALLock;

    static UINT ThreadFuncReadFile(LPVOID* lpvoid);
    static UINT ThreadFuncPaseNal(LPVOID* lpvoid);

    void ReadFile(void);
    void PaseNal(void);

    int m_nNalOffset;
    int m_nNalLen;
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnLvnItemActivateH264Nallist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawH264Nallist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnFileOpen();
    afx_msg void OnHelpAbout();
    afx_msg void OnHowtoUsage();
    afx_msg void OnLvnKeydownH264Nallist(NMHDR *pNMHDR, LRESULT *pResult);
};
