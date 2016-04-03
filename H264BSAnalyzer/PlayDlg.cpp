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
    m_nWidth = 0;
    m_nHeight = 0;
    m_nTotalFrame = 0;
    m_nFrameCount = 0;
    m_fFps = 0.0;
    m_pbBmpData = NULL;
    m_strFileUrl.Empty();
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

    // 贴图
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
    m_bPlay.EnableWindow(TRUE);
    m_bStop.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_STOP)));
    m_bStop.EnableWindow(TRUE);
    m_bNextFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NEXT)));
    m_bNextFrame.EnableWindow(TRUE);
    m_bSaveFrame.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_SAVE)));
    m_bSaveFrame.EnableWindow(TRUE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CPlayDlg::OnPaint()
{
    //picture控件背景色为黑色
    if (m_fShowBlack)
    //if (0)
    {
        CRect rtTop;
        CStatic *pWnd = (CStatic*)GetDlgItem(IDC_VIDEO);
        CDC *cDc = pWnd->GetDC();
        pWnd->GetClientRect(&rtTop);
        cDc->FillSolidRect(rtTop.left, rtTop.top, rtTop.Width(), rtTop.Height(),RGB(0,0,0));
        Invalidate(FALSE);
    }

    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialogEx::OnPaint()
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
    //BYTE* pbBmpData = NULL;
    //static BYTE pbBmpData[1920*1080*3 + 54] = {};
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
    m_strFileUrl = strFileName;
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

    m_nFrameCount = 0;

    this->SetWindowText(DLG_TITTLE);

    m_fPlayed = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

    m_pbBmpData = new BYTE[m_nWidth * m_nHeight * 3 + 54];
    if (m_pbBmpData == NULL)
    {
        MessageBox("Malloc buffer for RGB data failed.");
        return -1;
    }
    if (m_strFileUrl.IsEmpty())
    {
        MessageBox("Sorry, you open no file...");
        return -1;
    }
    if (m_fClosed)
    {
        ret = m_cDecoder.openVideoFile(m_strFileUrl.GetBuffer());
        if (ret < 0)
        {
            MessageBox("Sorry, open video decoder failed.");
            return -1;
        }
        m_fClosed = FALSE;
    }

    return 0;
}

void CPlayDlg::Finish()
{
    m_cDecoder.closeVideoFile();

    // 停止定时器
    KillTimer(1);

    m_fClosed = TRUE;

    m_fPlayed = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));
}

void CPlayDlg::Showing()
{
    BYTE* pRgbBuffer = NULL;
    int nSize = 0;
    int ret = 0;

    CString strTittle;
    ret = m_cDecoder.getFrame(NULL, &pRgbBuffer, &nSize);
    m_nFrameCount++;
    strTittle.Format("%d/%d  %0.2ffps --  %s", m_nFrameCount, m_nTotalFrame, m_fFps, DLG_TITTLE);
    this->SetWindowText(strTittle);

    CString strDebugInfo;
    strDebugInfo.Format("debug: getFrame %d ret: %d", m_nFrameCount, ret);
    GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);

    if (ret < 0) return;
    else if ( ret > 0)
    {
        Show(pRgbBuffer, nSize, m_nWidth, m_nHeight);
    }
    else
    {
        ret = m_cDecoder.getSkippedFrame(NULL, &pRgbBuffer, &nSize);

        CString strDebugInfo;
        strDebugInfo.Format("debug: getSkippedFrame %d ret: %d", m_nFrameCount, ret);
        GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);

        if ( ret > 0)
        {
            Show(pRgbBuffer, nSize, m_nWidth, m_nHeight);
        }
    }
}

// 窗口关闭
void CPlayDlg::OnClose()
{
    OnBnClickedBtStop();

    CDialogEx::OnClose();
}

void CPlayDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (m_nFrameCount >= m_nTotalFrame)
    {
        OnBnClickedBtStop();
        if (m_fLoop) 
        {
            m_fPlayed = TRUE;
            OnBnClickedBtPlay();
        }
        return;
    }

    Showing();
}

void CPlayDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
}

// 播放
void CPlayDlg::OnBnClickedBtPlay()
{
    if (m_strFileUrl.IsEmpty())
    {
        MessageBox("Sorry, you open no file.");
        return;
    }

    if (m_fClosed)
    {
        if (m_cDecoder.openVideoFile(m_strFileUrl.GetBuffer()) < 0)
        {
            MessageBox("Sorry, open video decoder failed.");
            return;
        }

        m_fClosed = FALSE;
    }

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

    m_fPlayed = ! m_fPlayed;

    /*
    CString strDebugInfo;
    strDebugInfo.Format("debug: play: %d", m_fPlayed);
    GetDlgItem(IDC_S_DEBUG)->SetWindowText(strDebugInfo);
    */
}

void CPlayDlg::OnBnClickedBtStop()
{
    m_nFrameCount = 0;
    Finish();
}

void CPlayDlg::OnBnClickedBtNext()
{
    KillTimer(1);

    BYTE* pRgbBuffer = NULL;
    int nSize = 0;
    CString strTittle;

    // loop
    if (m_fClosed)
    {
        if (m_cDecoder.openVideoFile(m_strFileUrl.GetBuffer()) < 0)
        {
            MessageBox("Sorry, open video decoder failed.");
            return;
        }

        m_fClosed = FALSE;
    }

    m_fPlayed = TRUE;
    m_bPlay.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_PLAY)));

    OnTimer(-1);
    
    return;
}

void CPlayDlg::OnBnClickedBtSave()
{

}


void CPlayDlg::OnBnClickedCkLoop()
{
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CK_LOOP);
    m_fLoop = pBtn->GetCheck();
}
