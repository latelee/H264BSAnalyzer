
// H264BSAnalyzerDlg.h : header file
//
#include "resource.h"
#include "stdafx.h"

#include <vector>
using std::vector;

#pragma once

#define MAX_URL_LENGTH 2000

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
    int AppendNLInfo(int nal_reference_idc,int nal_unit_type,int len,int data_lenth,int data_offset, int startcode);
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
    CMFCEditBrowseCtrl m_h264InputUrl;
    CEdit m_h264NalInfo;
    CListCtrl m_h264NalList;
    afx_msg void OnBnClickedH264InputurlOpen();

    // our own
private:
    int m_nNalIndex;
    char m_strFileUrl[MAX_URL_LENGTH];
      //Ò»ÌõPacket¼ÇÂ¼
    typedef struct NALInfo{
        int data_offset;
        int data_lenth;
    }NALInfo;
    vector<NALInfo> m_vNalInfoVector;

public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnLvnItemActivateH264Nallist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawH264Nallist(NMHDR *pNMHDR, LRESULT *pResult);
};
