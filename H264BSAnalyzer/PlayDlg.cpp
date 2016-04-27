// PlayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "H264BSAnalyzer.h"
#include "PlayDlg.h"
#include "afxdialogex.h"

// CPlayDlg dialog

IMPLEMENT_DYNAMIC(CPlayDlg, CDialogEx)

CPlayDlg::CPlayDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPlayDlg::IDD, pParent)
{
    m_fShowBlack = FALSE;
    m_fPlayed = FALSE;
    m_fClosed = TRUE;
    m_fLoop = FALSE;
    m_fInit = FALSE;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nTotalFrame = 0;
    m_nFrameCount = 0;
    m_fFps = 0.0;
    m_pbBmpData = NULL;
    m_pbRgbBuffer = NULL;
    m_iBmpSize = 0;
    m_iRgbSize = 0;
    m_strPathName.Empty();
    m_pParentWnd = NULL;
}

CPlayDlg::~CPlayDlg()
{
    if (m_pbBmpData)
    {
        delete m_pbBmpData;
        m_pbBmpData = NULL;
    }
}

void CPlayDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BT_PLAY, m_bPlay);
    DDX_Control(pDX, IDC_BT_SAVE, m_bSaveFrame);
    DDX_Control(pDX, IDC_BT_STOP, m_bStop);
    DDX_Control(pDX, IDC_BT_NEXT, m_bNextFrame);
}

BEGIN_MESSAGE_MAP(CPlayDlg, CDialogEx)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BT_PLAY, &CPlayDlg::OnBnClickedBtPlay)
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BT_NEXT, &CPlayDlg::OnBnClickedBtNext)
    ON_BN_CLICKED(IDC_BT_SAVE, &CPlayDlg::OnBnClickedBtSave)
    ON_BN_CLICKED(IDC_BT_STOP, &CPlayDlg::OnBnClickedBtStop)
    ON_BN_CLICKED(IDC_CK_LOOP, &CPlayDlg::OnBnClickedCkLoop)
END_MESSAGE_MAP()


// CPlayDlg message handlers
BOOL CPlayDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_vStartX.resize(2);
    m_vStartX[0].push_back(IDC_BT_PLAY);
    m_vStartX[0].push_back(IDC_BT_STOP);
    m_vStartX[0].push_back(IDC_BT_NEXT);
    m_vStartX[0].push_back(IDC_BT_SAVE);
    m_vStartX[0].push_back(IDC_CK_LOOP);
    m_vStartX[0].push_back(IDC_S_DEBUG);

    CRect rect;
    for (unsigned int i = 0; i < m_vStartX[0].size(); i++)
    {
        GetDlgItem(m_vStartX[0][i])->GetWindowRect(rect);
        ScreenToClient(rect);
        m_vStartX[1].push_back(rect.left);
    }

    // 贴图
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bPlay.EnableWindow(TRUE);
    m_bStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_STOP)));
    m_bStop.EnableWindow(TRUE);
    m_bNextFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NEXT)));
    m_bNextFrame.EnableWindow(TRUE);
    m_bSaveFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SAVE)));
    m_bSaveFrame.EnableWindow(TRUE);

    m_fInit = TRUE;

    return TRUE;  // return TRUE unless you set the focus to a control

}

void CPlayDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    Show(m_pbRgbBuffer, m_iRgbSize, m_nWidth, m_nHeight);
}


inline void RenderBitmap(CWnd *pWnd, Bitmap* pbmp)
{
    RECT rect;
    pWnd->GetClientRect( &rect );

    Graphics grf( pWnd->m_hWnd );
    if ( grf.GetLastStatus() == Ok )
    {
        Rect rc( rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top );

        grf.DrawImage(pbmp, rc);
    }
}

// 显示图片
void CPlayDlg::ShowPicture(BYTE* pbData, int iSize)
{
    if (pbData == NULL) return;
    CWnd* pWnd=GetDlgItem(IDC_VIDEO);   // IDC_VIDEO：picture contral 控件ID
    IStream* pPicture = NULL;
    CreateStreamOnHGlobal(NULL,TRUE,&pPicture);
    if( pPicture != NULL )
    {
        pPicture->Write(pbData,  iSize, NULL);
        LARGE_INTEGER liTemp = { 0 };
        pPicture->Seek(liTemp, STREAM_SEEK_SET, NULL);
        Bitmap TempBmp(pPicture);
        RenderBitmap(pWnd, &TempBmp);
    }
    if(pPicture != NULL)
    {
        pPicture->Release();
        pPicture = NULL;
    }

    m_fShowBlack = FALSE;
}

void CPlayDlg::Show(BYTE* pbData, int nSize, int nWidth, int nHeight)
{
    MYBITMAPFILEHEADER bmpHeader;
    MYBITMAPINFOHEADER bmpInfo;

    // 先添加BMP头
    bmpHeader.bfType = 'MB';
    bmpHeader.bfSize = nSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpInfo.biSize   = sizeof(BITMAPINFOHEADER);
    bmpInfo.biWidth  = nWidth;
    bmpInfo.biHeight = -nHeight;    // note BMP图像是倒过来的
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = 24;
    bmpInfo.biCompression = BI_RGB;
    bmpInfo.biSizeImage   = 0;//nSize;
    bmpInfo.biXPelsPerMeter = 0;
    bmpInfo.biYPelsPerMeter = 0;
    bmpInfo.biClrUsed = 0;
    bmpInfo.biClrImportant = 0;

    memcpy(m_pbBmpData, &bmpHeader, sizeof(BITMAPFILEHEADER));
    memcpy(m_pbBmpData+sizeof(BITMAPFILEHEADER), &bmpInfo, sizeof(BITMAPINFOHEADER));
    memcpy(m_pbBmpData+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), pbData, nSize);
    // 显示
    ShowPicture(m_pbBmpData, bmpHeader.bfSize);
}

int CPlayDlg::SetVideoInfo(CString strFileName, int nWidth, int nHeight, int nTotalFrame, float nFps)
{
    int ret = 0;
    m_strPathName = strFileName;
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    m_nTotalFrame = nTotalFrame;
    m_fFps = nFps;

    if (m_fFps <= 0 ) m_fFps = 25.0;

    if (m_pbBmpData)
    {
        delete m_pbBmpData;
        m_pbBmpData = NULL;
    }

    this->SetWindowText(DLG_TITTLE);

    m_fPlayed = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

    m_iBmpSize = m_nWidth * m_nHeight * 3 + 54;
    m_pbBmpData = new BYTE[m_iBmpSize];
    if (m_pbBmpData == NULL)
    {
        MessageBox("Malloc buffer for RGB data failed.");
        return -1;
    }
    if (m_strPathName.IsEmpty())
    {
        MessageBox("Sorry, you open no file...");
        return -1;
    }
    if (m_fClosed)
    {
        ret = m_cDecoder.openVideoFile(m_strPathName.GetBuffer());
        if (ret < 0)
        {
            MessageBox("Sorry, open video decoder failed.");
            return -1;
        }
        m_fClosed = FALSE;
        m_nFrameCount = 0;
    }

    return 0;
}

void CPlayDlg::ShowFirstFrame()
{
    OnBnClickedBtNext();
}

//picture控件背景色为黑色
void CPlayDlg::SetBlack()
{
    CRect rtTop;
    CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
    CDC *cDc = pWnd->GetDC();
    pWnd->GetClientRect(&rtTop);
    cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
    Invalidate(FALSE);
}

void CPlayDlg::ShowingFrame()
{
    int ret = 0;

    CString strTittle;
    ret = m_cDecoder.getFrame(NULL, &m_pbRgbBuffer, &m_iRgbSize);
    m_nFrameCount++;
    strTittle.Format("%d/%d  %0.2ffps --  %s", m_nFrameCount, m_nTotalFrame, m_fFps, DLG_TITTLE);
    this->SetWindowText(strTittle);

#if 0
    CString strDebugInfo;
    strDebugInfo.Format("debug: %d ret: %d size: %d", m_nFrameCount, ret,m_iRgbSize);
    GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);
#endif

    if (ret < 0) return;
    else if ( ret > 0)
    {
        Show(m_pbRgbBuffer, m_iRgbSize, m_nWidth, m_nHeight);
    }
    else
    {
        ret = m_cDecoder.getSkippedFrame(NULL, &m_pbRgbBuffer, &m_iRgbSize);
        m_iRgbSize = m_iRgbSize ? m_iRgbSize: m_nWidth*m_nHeight*3; // 解码缓存的帧时，获取的size为0，所以这样判断

#if 0
        CString strDebugInfo;
        strDebugInfo.Format("skip %d ret: %d size: %d", m_nFrameCount, ret, m_iRgbSize);
        GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);
#endif
        if ( ret > 0)
        {
            Show(m_pbRgbBuffer, m_iRgbSize, m_nWidth, m_nHeight);
        }
    }
}

// 文件名带%d %05d等表示保存所有图片
static bool IsSingleFile(const char* filename)
{
    std::string pathname = filename;

    std::string::size_type pos = pathname.find_first_of('%');
    if (pos == std::string::npos)
    {
        return true;
    }
    std::string::size_type pos1 = pathname.find_first_of('d', pos);

    if (pos1 == std::string::npos)
    {
        return true;
    }

    return false;
}

int CPlayDlg::SaveYUVFile(const char* pFileName)
{
    int ret = 0;
    if (IsSingleFile(pFileName))
    {
        m_cDecoder.writeYUVFile(pFileName);
    }
    else
    {
        CH264Decoder foo;
        foo.openVideoFile(m_strPathName);
        char szFileName[256] = {0};
        int cnt = 1;
        while (foo.getFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeYUVFile(szFileName);
        }
        while (foo.getSkippedFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeYUVFile(szFileName);
        }
    }

    return ret;
}

int CPlayDlg::SaveBMPFile(const char* pFileName)
{
    int ret = 0;
    if (IsSingleFile(pFileName))
    {
        m_cDecoder.writeBMPFile2(pFileName);
    }
    else
    {
        CH264Decoder foo;
        foo.openVideoFile(m_strPathName);
        char szFileName[256] = {0};
        int cnt = 1;
        while (foo.getFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeBMPFile2(szFileName);
        }
        while (foo.getSkippedFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeBMPFile2(szFileName);
        }
    }

    return ret;
}

int CPlayDlg::SaveJPGFile(const char* pFileName)
{
    int ret = 0;
    if (IsSingleFile(pFileName))
    {
        m_cDecoder.writeJPGFile2(pFileName);
    }
    else
    {
        CH264Decoder foo;
        foo.openVideoFile(m_strPathName);
        char szFileName[256] = {0};
        int cnt = 1;
        while (foo.getFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeJPGFile2(szFileName);
        }
        while (foo.getSkippedFrame() > 0)
        {
            sprintf(szFileName, pFileName, cnt++);
            foo.writeJPGFile2(szFileName);
        }
    }

    return ret;
}

int CPlayDlg::SaveVideoFile(const char* pFileName)
{
    int ret = 0;
    H264BS2Video cSaveVideo;

    ret = cSaveVideo.openVideoFile(m_strPathName.GetBuffer(), pFileName, m_nWidth, m_nHeight);
    if (ret < 0)
    {
        MessageBox("Open video file failed");
        return -1;
    }
    ret = cSaveVideo.writeFrame();
    if (ret < 0)
    {
        MessageBox("Write to video file failed");
        return -1;
    }
    
    cSaveVideo.close();

    return 0;
}

// 窗口关闭
void CPlayDlg::OnClose()
{
    OnBnClickedBtStop();

    CDialogEx::OnClose();
}


void CPlayDlg::Pause()
{
    KillTimer(1);

    m_fPlayed = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

}

void CPlayDlg::CloseVideo()
{
    if (m_fClosed == TRUE)
    {
        m_nFrameCount = 0;
        m_cDecoder.closeVideoFile();
    }  
}

// 使用关闭文件再打开的方式，是因为没有封装ffmpeg的复位操作
void CPlayDlg::ReOpenVideo()
{
    CloseVideo();

    if (m_fClosed == TRUE)
    {
        if (m_cDecoder.openVideoFile(m_strPathName.GetBuffer()) < 0)
        {
            MessageBox("Sorry, open video decoder failed.");
            return;
        }

        m_fClosed = FALSE;
    }
}

void CPlayDlg::OnBnClickedBtStop()
{
    Pause();

    m_fClosed = TRUE;

    CloseVideo();

    CString strTittle;
    strTittle.Format("%d/%d  %0.2ffps --  %s", m_nFrameCount, m_nTotalFrame, m_fFps, DLG_TITTLE);
    this->SetWindowText(strTittle);

    SetBlack();
}

void CPlayDlg::OnTimer(UINT_PTR nIDEvent)
{
    ShowingFrame();

    if (m_nFrameCount >= m_nTotalFrame)
    {
        Pause();
        m_fClosed = TRUE;
        if (m_fLoop) 
        {
            m_fPlayed = TRUE;
            OnBnClickedBtPlay();
        }
        return;
    }
}

void CPlayDlg::OnSize(UINT nType, int cx, int cy)
{
    if (!m_fInit) return;

    CDialogEx::OnSize(nType, cx, cy);

    CWnd *pWnd = GetDlgItem(IDC_VIDEO);
    if (pWnd)
    {
        pWnd->MoveWindow(0, 0, cx, cy-26-5);
        pWnd->Invalidate();
        pWnd->UpdateData();
    }

    int startx = 1;
    // 水平位置相同的按钮
    for (unsigned int i = 0; i < m_vStartX[0].size(); i++)
    {
        pWnd = GetDlgItem(m_vStartX[0][i]);
        if (pWnd)
        {
            pWnd->SetWindowPos(NULL,startx+m_vStartX[1][i],cy-26,0,0,SWP_NOZORDER|SWP_NOSIZE);
        }
    }
}

// 播放
void CPlayDlg::OnBnClickedBtPlay()
{
    if (m_strPathName.IsEmpty())
    {
        MessageBox("Sorry, you open no file.");
        return;
    }

    ReOpenVideo();

    if (m_fPlayed)
    {
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PAUSE))); // show pause...
        SetTimer(1,(UINT)((float)1000/m_fFps),NULL);
    }
    else
    {
        m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY))); // show play...
        KillTimer(1);
    }

    m_fPlayed = !m_fPlayed;
}

void CPlayDlg::OnBnClickedBtNext()
{
    Pause();

    // loop
    ReOpenVideo();

    OnTimer(-1);
    
    return;
}


void CPlayDlg::OnBnClickedBtSave()
{
    char szFilter[] = "YUV File(*.yuv)|*.yuv|"
                         "BMP File(*.bmp)|*.bmp|"
                         "JPG File(*.jpg)|*.jpg|"
                         "AVI File(*.avi)|*.avi|"
                         "MP4 File(*.mp4)|*.mp4|"
                         "MOV File(*.mov)|*.mov|"
                         "||";
    char szFileName[128] = "foobar";
    char* pExt = _T("yuv");

    CFile cFile;
    CString strFile;

    if (m_nFrameCount < 1)
    {
        MessageBox("Sorry, can't save frame.");
        return;
    }

    Pause();

#if 0
    std::string pathname = m_strPathName;

    std::string::size_type pos = pathname.find_last_of('.');
    if (pos != std::string::npos)
    {
        
    }
    std::string extname = pathname.substr(pos+1);

    std::string::size_type pos1 = pathname.find_last_of('\\');
    std::string filename = pathname.substr(pos1+1, pos-pos1-1);
    if (pos != std::string::npos)
    {
        
    }
    std::string dirname = filename.substr(0, pos1);
#endif
    
    _splitpath(m_strPathName, NULL, NULL, szFileName, NULL);
    strFile.Format(_T("%s_%d.%s"), szFileName, m_nFrameCount, pExt);

    CFileDialog fileDlg(FALSE, _T("Save File"), strFile.GetBuffer(), OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("Save File");
    if (fileDlg.DoModal() != IDOK)
        return;

    CString strSaveFile = fileDlg.GetPathName();
    CString strExt = fileDlg.GetFileExt();

    int ret = 0;
    if (!strExt.CompareNoCase(_T("yuv")))
    {
        ret = SaveYUVFile(strSaveFile.GetBuffer());
    }
    else if (!strExt.CompareNoCase(_T("bmp")))
    {
        ret = SaveBMPFile(strSaveFile.GetBuffer());
    }
    else if (!strExt.CompareNoCase(_T("jpg")))
    {
        ret = SaveJPGFile(strSaveFile.GetBuffer());
    }
    else
    {
        ret = SaveVideoFile(strSaveFile.GetBuffer());
    }

    if (ret == 0)
    {
        MessageBox("Job done.");
    }

#if 0
    CString strDebugInfo;
    strDebugInfo.Format("debug:  ret: %d, file: %s", ret, strSaveFile);
    GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);
#endif
}


void CPlayDlg::OnBnClickedCkLoop()
{
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CK_LOOP);
    m_fLoop = pBtn->GetCheck();
}

BOOL CPlayDlg::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        // press esc..
        case VK_ESCAPE:
            OnBnClickedBtStop();
            break;
        default:
            break;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}