#pragma once

#include <string>
#include <vector>
#include <map>

#include "H264Decode.h"
#include "H264ToVideo.h"

#define DLG_TITTLE "Play"

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
    // 从主窗口拿到视频文件信息
    int SetVideoInfo(CString strFileName, int nWidth, int nHeight, int nTotalFrame, float nFps);
    void ShowFirstFrame();
    void SetBlack();

private:
    void ShowPicture(BYTE* pbData, int iSize);
    void Show(BYTE* pbData, int nSize, int nWidth, int nHeight);

    void ShowingFrame();
    void Pause();
    void CloseVideo();
    void ReOpenVideo();

    int SaveYUVFile(const char* pFileName);
    int SaveBMPFile(const char* pFileName);
    int SaveJPGFile(const char* pFileName);
    int SaveVideoFile(const char* pFileName);
    
private:
    BOOL m_fShowBlack;
    BOOL m_fPlayed;
    BOOL m_fClosed;
    BOOL m_fLoop;
    BOOL m_fInit;
    INT m_nWidth;
    INT m_nHeight;
    INT m_nTotalFrame;
    INT m_nFrameCount;
    float m_fFps;
    BYTE* m_pbBmpData;
    INT m_iBmpSize;
    CString m_strPathName;   // 视频文件

    CH264Decoder m_cDecoder;    // 解码器

    std::vector<std::vector<int> > m_vStartX;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg void OnBnClickedBtPlay();
    afx_msg void OnClose();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CButton m_bPlay;
    CButton m_bSaveFrame;
    CButton m_bStop;
    CButton m_bNextFrame;
    afx_msg void OnBnClickedBtNext();
    afx_msg void OnBnClickedBtSave();
    afx_msg void OnBnClickedBtStop();
    afx_msg void OnBnClickedCkLoop();
};
