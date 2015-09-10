
// H264BSAnalyzerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "H264BSAnalyzer.h"
#include "H264BSAnalyzerDlg.h"
#include "afxdialogex.h"

#include "NaLParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    RECT m_pRectLink;
    RECT m_pRectLink1;
public:
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


// CH264BSAnalyzerDlg dialog




CH264BSAnalyzerDlg::CH264BSAnalyzerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CH264BSAnalyzerDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hFileThread = INVALID_HANDLE_VALUE;
    m_hNALThread = INVALID_HANDLE_VALUE;
    m_hFileLock = INVALID_HANDLE_VALUE;
    m_hNALLock = INVALID_HANDLE_VALUE;
}

void CH264BSAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_H264_INPUTURL, m_h264InputUrl);
    DDX_Control(pDX, IDC_H264_NALINFO, m_h264NalInfo);
    DDX_Control(pDX, IDC_H264_NALLIST, m_h264NalList);
    DDX_Control(pDX, IDC_EDIT_FILE, m_edFileUrl);
    DDX_Control(pDX, IDC_EDIT_HEX, m_edHexInfo);
    DDX_Control(pDX, IDC_CB_NAL, m_cbNalNum);
}

BEGIN_MESSAGE_MAP(CH264BSAnalyzerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_H264_INPUTURL_OPEN, &CH264BSAnalyzerDlg::OnBnClickedH264InputurlOpen)
    ON_WM_DROPFILES()
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnLvnItemActivateH264Nallist)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnNMCustomdrawH264Nallist)
    ON_COMMAND(ID_FILE_OPEN32771, &CH264BSAnalyzerDlg::OnFileOpen)
    ON_COMMAND(ID_HELP_ABOUT, &CH264BSAnalyzerDlg::OnHelpAbout)
    ON_COMMAND(ID_HOWTO_USAGE, &CH264BSAnalyzerDlg::OnHowtoUsage)
    ON_NOTIFY(LVN_KEYDOWN, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnLvnKeydownH264Nallist)
END_MESSAGE_MAP()

// CH264BSAnalyzerDlg message handlers

BOOL CH264BSAnalyzerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
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

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    //整行选择；有表格线；表头；单击激活
    DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;
    //报表风格；单行选择；高亮显示选择行
    m_h264NalList.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
    m_h264NalList.SetExtendedStyle(dwExStyle);

    // 左对齐
    m_h264NalList.InsertColumn(0,_T("No."),LVCFMT_LEFT,50,0);
    m_h264NalList.InsertColumn(1,_T("Offset"),LVCFMT_LEFT,70,0);
    m_h264NalList.InsertColumn(2,_T("Length"),LVCFMT_LEFT,60,0);
    m_h264NalList.InsertColumn(3,_T("Start Code"),LVCFMT_LEFT,80,0);
    m_h264NalList.InsertColumn(4,_T("NAL Type"),LVCFMT_LEFT,180,0);
    m_h264NalList.InsertColumn(5,_T("Info"),LVCFMT_LEFT,80,0);

    m_cbNalNum.SetCurSel(0);

    m_nSliceIndex = 0;

    m_nValTotalNum = 0;

    m_strFileUrl.Empty();

    m_edHexInfo.SetOptions(1, 1, 1, 1);
    m_edHexInfo.SetBPR(16); // 16字节

    if (m_hFileLock == INVALID_HANDLE_VALUE)
    {
        m_hFileLock = CreateEvent(NULL,FALSE,FALSE,NULL);
    }
    if (m_hNALLock == INVALID_HANDLE_VALUE)
    {
        m_hNALLock = CreateEvent(NULL,FALSE,FALSE,NULL);
    }
    if (m_hFileThread == INVALID_HANDLE_VALUE)
    {
        m_hFileThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFuncReadFile, this, NULL, NULL );
    }
    // 暂不使用
    /*
    if (m_hNALThread == INVALID_HANDLE_VALUE)
    {
        m_hNALThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFuncPaseNal, this, NULL, NULL );
    }
    */
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CH264BSAnalyzerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CH264BSAnalyzerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CH264BSAnalyzerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


//添加一条记录
int CH264BSAnalyzerDlg::ShowNLInfo(NALU_t* nalu)
{
    CString strTempIndex;
    CString strOffset;
    CString strNalLen;
    CString strStartCode;
    CString strNalUnitType;
    CString strNalInfo;
    int nIndex=0;

    if (nalu->type == 0)
    {
        // NAL单元类型
        switch (nalu->nal_unit_type)
        {
        case 0:
            strNalUnitType.Format(_T("Unspecified"));
            break;
        case 1:
            strNalUnitType.Format(_T("Coded slice of a non-IDR picture"));
            switch (nalu->slice_type)
            {
            case 0:
            case 5:
                strNalInfo.Format(_T("P Slice #%d"), m_nSliceIndex);
                break;
            case 1:
            case 6:
                strNalInfo.Format(_T("B Slice #%d"), m_nSliceIndex);
                break;
            case 2:
            case 7:
                strNalInfo.Format(_T("I Slice #%d"), m_nSliceIndex);
                break;
            }
            m_nSliceIndex++;
            break;
        case 2:
            strNalUnitType.Format(_T("DPA"));
            break;
        case 3:
            strNalUnitType.Format(_T("DPB"));
            break;
        case 4:
            strNalUnitType.Format(_T("DPC"));
            break;
        case 5:
            strNalUnitType.Format(_T("Coded slice of an IDR picture"));
            strNalInfo.Format(_T("IDR #%d"), m_nSliceIndex);
            m_nSliceIndex++;
            break;
        case 6:
            strNalUnitType.Format(_T("Supplemental enhancement information"));
            strNalInfo.Format(_T("SEI"));
            break;
        case 7:
            strNalUnitType.Format(_T("Sequence parameter set"));
            strNalInfo.Format(_T("SPS"));
            break;
        case 8:
            strNalUnitType.Format(_T("Picture parameter set"));
            strNalInfo.Format(_T("PPS"));
            break;
        case 9:
            strNalUnitType.Format(_T("Access UD"));
            strNalInfo.Format(_T("AUD"));
            break;
        case 10:
            strNalUnitType.Format(_T("END_SEQUENCE"));
            break;
        case 11:
            strNalUnitType.Format(_T("END_STREAM"));
            break;
        case 12:
            strNalUnitType.Format(_T("FILLER_DATA"));
            break;
        case 13:
            strNalUnitType.Format(_T("SPS_EXT"));
            break;
        case 19:
            strNalUnitType.Format(_T("AUXILIARY_SLICE"));
            break;
        default:
            strNalUnitType.Format(_T("Other"));
            break;
        }
    }
    else
    {
        // NAL单元类型
        switch (nalu->nal_unit_type)
        {
        case NAL_UNIT_CODED_SLICE_TRAIL_N:
        case NAL_UNIT_CODED_SLICE_TRAIL_R:
            strNalUnitType.Format(_T("Coded slice segment of a non-TSA, non-STSA trailing picture"));
            // todo
            switch (nalu->slice_type)
            {
            case 0:
            case 5:
                strNalInfo.Format(_T("P Slice #%d"), m_nSliceIndex);
                break;
            case 1:
            case 6:
                strNalInfo.Format(_T("B Slice #%d"), m_nSliceIndex);
                break;
            case 2:
            case 7:
                strNalInfo.Format(_T("I Slice #%d"), m_nSliceIndex);
                break;
            }
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_TSA_N:
        case NAL_UNIT_CODED_SLICE_TSA_R:
            strNalUnitType.Format(_T("Coded slice segment of a TSA picture"));
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_RADL_N:
        case NAL_UNIT_CODED_SLICE_RADL_R:
            strNalUnitType.Format(_T("Coded slice segment of a TSA picture"));
            m_nSliceIndex++;
            break;
        // todo...
        case NAL_UNIT_CODED_SLICE_IDR_W_RADL:
        case NAL_UNIT_CODED_SLICE_IDR_N_LP:
            strNalUnitType.Format(_T("Coded slice of an IDR picture"));
            strNalInfo.Format(_T("IDR #%d"), m_nSliceIndex);
            m_nSliceIndex++;
            break;
        case NAL_UNIT_PREFIX_SEI:
        case NAL_UNIT_SUFFIX_SEI:
            strNalUnitType.Format(_T("Supplemental enhancement information"));
            strNalInfo.Format(_T("SEI"));
            break;
        case NAL_UNIT_VPS:
            strNalUnitType.Format(_T("Video parameter set"));
            strNalInfo.Format(_T("VPS"));
            break;
        case NAL_UNIT_SPS:
            strNalUnitType.Format(_T("Sequence parameter set"));
            strNalInfo.Format(_T("SPS"));
            break;
        case NAL_UNIT_PPS:
            strNalUnitType.Format(_T("Picture parameter set"));
            strNalInfo.Format(_T("PPS"));
            break;
        case NAL_UNIT_AUD:
            strNalUnitType.Format(_T("Access UD"));
            strNalInfo.Format(_T("AUD"));
            break;
        case NAL_UNIT_EOS:
            strNalUnitType.Format(_T("END_SEQUENCE"));
            break;
        case NAL_UNIT_EOB:
            strNalUnitType.Format(_T("END_STREAM"));
            break;
        case NAL_UNIT_FILLER_DATA:
            strNalUnitType.Format(_T("FILLER_DATA"));
            break;
        default:
            strNalUnitType.Format(_T("Unknown"));
            break;
        }
    }


    // 序号
    strTempIndex.Format(_T("%d"),nalu->num + 1);  // 向量中的序号从0开始
    // 数据偏移
    strOffset.Format(_T("%08X"), nalu->data_offset);
    // 长度
    strNalLen.Format(_T("%d"),nalu->len);
    // 起始码
    strStartCode.Format(_T("%s"), nalu->startcode_buf);

    //获取当前记录条数
    nIndex=m_h264NalList.GetItemCount();
    //“行”数据结构
    LV_ITEM lvitem;
    lvitem.mask=LVIF_TEXT;
    lvitem.iItem=nIndex;
    lvitem.iSubItem=0;
    //注：vframe_index不可以直接赋值！
    //务必使用f_index执行Format!再赋值！
    lvitem.pszText=(LPSTR)strTempIndex.GetBuffer();;

    //------------------------显示在List中
    m_h264NalList.InsertItem(&lvitem);
    m_h264NalList.SetItemText(nIndex,1,strOffset);
    m_h264NalList.SetItemText(nIndex,2,strNalLen);
    m_h264NalList.SetItemText(nIndex,3,strStartCode);
    m_h264NalList.SetItemText(nIndex,4,strNalUnitType);
    m_h264NalList.SetItemText(nIndex,5,strNalInfo);

    return TRUE;
}

void CH264BSAnalyzerDlg::SystemClear()
{
    //m_vNalInfoVector.clear();
    m_vNalTypeVector.clear();
    m_h264NalList.DeleteAllItems();
    m_nSliceIndex = 0;
    m_nValTotalNum = 0;
}

// 打开H264码流文件
void CH264BSAnalyzerDlg::OnBnClickedH264InputurlOpen()
{
    // TODO: Add your control notification handler code here
    SystemClear();
    UpdateData();

        ////////////////
    // 测试用
#if 0
    //char a[16]={0};
    char pszBuffer[1000] = "hello world 1234567 you go to hell hahahahahah";
    m_edHexInfo.SetOptions(1, 1, 1, 1);
    m_edHexInfo.SetBPR(16);
    m_edHexInfo.SetData((LPBYTE)pszBuffer, 100, strlen(pszBuffer));
    m_edHexInfo.SetFocus();

    CString aaa;
    GetDlgItem(IDC_CB_NAL)->GetWindowTextA(aaa);
    MessageBox(aaa);
    return;
#endif

    CString strFilePath;
    CString strSimpleInfo;
    CString strProfileInfo;
    CString strVideoFormat;
    CString strMaxNalNum;
    int nMaxNalNum = -1;
    SPSInfo_t sps = {0};
    PPSInfo_t pps = {0};

    m_edFileUrl.GetWindowText(m_strFileUrl);

    // 输入非数字时，获取的nMaxNalNum为-1，则显示所有的NAL
    m_cbNalNum.GetWindowText(strMaxNalNum);
    sscanf((char*)strMaxNalNum.GetBuffer(), "%d", &nMaxNalNum);

    // test
    //m_strFileUrl.Format("%s", "../foreman_cif.h264");
    

    if(m_strFileUrl.IsEmpty()==TRUE)
    {
        AfxMessageBox(_T("文件路径为空，请打开文件！！"));
        return;
    }

    strcpy(str_szFileUrl, (char*)m_strFileUrl.GetBuffer());

    SetEvent(m_hFileLock);
#if 0
    h264_nal_probe(str_szFileUrl, m_vNalTypeVector, nMaxNalNum);

    m_nValTotalNum = m_vNalTypeVector.size();

    for (int i = 0; i < m_nValTotalNum; i++)
    {
        // 解析SPS
        if (m_vNalTypeVector[i].nal_unit_type == 7)
        {
            h264_sps_parse(str_szFileUrl, m_vNalTypeVector[i].data_offset, m_vNalTypeVector[i].len, sps);
        }
        // 解析PPS
        if (m_vNalTypeVector[i].nal_unit_type == 8)
        {
            h264_pps_parse(str_szFileUrl, m_vNalTypeVector[i].data_offset, m_vNalTypeVector[i].len, pps);
        }
        ShowNLInfo(&m_vNalTypeVector[i]);
    }
    // profile类型
    switch (sps.profile_idc)
    {
    case 66:
        strProfileInfo.Format(_T("Baseline"));
        break;
    case 77:
        strProfileInfo.Format(_T("Main"));
        break;
    case 88:
        strProfileInfo.Format(_T("Extended"));
        break;
    case 100:
        strProfileInfo.Format(_T("High"));
        break;
    case 110:
        strProfileInfo.Format(_T("High 10"));
        break;
    case 122:
        strProfileInfo.Format(_T("High 422"));
        break;
    case 144:
        strProfileInfo.Format(_T("High 444"));
        break;
    default:
        strProfileInfo.Format(_T("Unkown"));
        break;
    }
    switch (sps.chroma_format_idc)
    {
    case 1:
        strVideoFormat.Format(_T("YUV420"));
        break;
    case 2:
        strVideoFormat.Format(_T("YUV422"));
        break;
    case 3:
        strVideoFormat.Format(_T("YUV444"));
        break;
    case 0:
        strVideoFormat.Format(_T("monochrome"));
        break;
    default:
        strVideoFormat.Format(_T("Unkown"));
        break;
    }
        
    // todo
    /*
    "Video Format: xxx\r\n"
    */
    strSimpleInfo.Format(
        "File name: %s\r\n"
        "Picture Size: %dx%d\r\n"
        " - Cropping Left        : %d\r\n"
        " - Cropping Right      : %d\r\n"
        " - Cropping Top        : %d\r\n"
        " - Cropping Bottom   : %d\r\n"
        "Video Format: %s\r\n"
        "Stream Type: %s Profile @ Level %d\r\n"
        "Encoding Type: %s\r\n"
        "Max fps: %.03f\r\n",
        m_strFileUrl,
        sps.width, sps.height,
        sps.crop_left, sps.crop_right,
        sps.crop_top, sps.crop_bottom,
        strVideoFormat,
        strProfileInfo, sps.level_idc,
        pps.encoding_type ? "CABAC" : "CAVLC",
        sps.max_framerate
        );
    GetDlgItem(IDC_EDIT_SIMINFO)->SetWindowText(strSimpleInfo);
#endif
}

UINT CH264BSAnalyzerDlg::ThreadFuncReadFile(LPVOID* lpvoid)
{
    CH264BSAnalyzerDlg* dlg = (CH264BSAnalyzerDlg*)lpvoid;
    dlg->ReadFile();
    return 0;
}

void CH264BSAnalyzerDlg::ReadFile(void)
{
    CString strFilePath;
    CString strSimpleInfo;
    CString strProfileInfo;
    CString strVideoFormat;
    CString strMaxNalNum;
    int nMaxNalNum = -1;
    SPSInfo_t sps = {0};
    PPSInfo_t pps = {0};

    while (true)
    {
        WaitForSingleObject(m_hFileLock, INFINITE);

        h264_nal_probe(str_szFileUrl, m_vNalTypeVector, nMaxNalNum);

        m_nValTotalNum = m_vNalTypeVector.size();

        for (int i = 0; i < m_nValTotalNum; i++)
        {
            // 解析SPS
            if (m_vNalTypeVector[i].nal_unit_type == 7)
            {
                h264_sps_parse(str_szFileUrl, m_vNalTypeVector[i].data_offset, m_vNalTypeVector[i].len, sps);
            }
            // 解析PPS
            if (m_vNalTypeVector[i].nal_unit_type == 8)
            {
                h264_pps_parse(str_szFileUrl, m_vNalTypeVector[i].data_offset, m_vNalTypeVector[i].len, pps);
            }
            ShowNLInfo(&m_vNalTypeVector[i]);
        }
        // profile类型
        switch (sps.profile_idc)
        {
        case 66:
            strProfileInfo.Format(_T("Baseline"));
            break;
        case 77:
            strProfileInfo.Format(_T("Main"));
            break;
        case 88:
            strProfileInfo.Format(_T("Extended"));
            break;
        case 100:
            strProfileInfo.Format(_T("High"));
            break;
        case 110:
            strProfileInfo.Format(_T("High 10"));
            break;
        case 122:
            strProfileInfo.Format(_T("High 422"));
            break;
        case 144:
            strProfileInfo.Format(_T("High 444"));
            break;
        default:
            strProfileInfo.Format(_T("Unkown"));
            break;
        }
        switch (sps.chroma_format_idc)
        {
        case 1:
            strVideoFormat.Format(_T("YUV420"));
            break;
        case 2:
            strVideoFormat.Format(_T("YUV422"));
            break;
        case 3:
            strVideoFormat.Format(_T("YUV444"));
            break;
        case 0:
            strVideoFormat.Format(_T("monochrome"));
            break;
        default:
            strVideoFormat.Format(_T("Unkown"));
            break;
        }
        
        // todo
        /*
        "Video Format: xxx\r\n"
        */
        strSimpleInfo.Format(
            "File name: %s\r\n"
            "Picture Size: %dx%d\r\n"
            " - Cropping Left        : %d\r\n"
            " - Cropping Right      : %d\r\n"
            " - Cropping Top        : %d\r\n"
            " - Cropping Bottom   : %d\r\n"
            "Video Format: %s\r\n"
            "Stream Type: %s Profile @ Level %d\r\n"
            "Encoding Type: %s\r\n"
            "Max fps: %.03f\r\n",
            m_strFileUrl,
            sps.width, sps.height,
            sps.crop_left, sps.crop_right,
            sps.crop_top, sps.crop_bottom,
            strVideoFormat,
            strProfileInfo, sps.level_idc,
            pps.encoding_type ? "CABAC" : "CAVLC",
            sps.max_framerate
            );
        GetDlgItem(IDC_EDIT_SIMINFO)->SetWindowText(strSimpleInfo);
    }
}

UINT CH264BSAnalyzerDlg::ThreadFuncPaseNal(LPVOID* lpvoid)
{
    CH264BSAnalyzerDlg* dlg = (CH264BSAnalyzerDlg*)lpvoid;
    dlg->PaseNal();
    return 0;
}

void CH264BSAnalyzerDlg::PaseNal(void)
{
    int ret = 0;
    while (true)
    {
        WaitForSingleObject(m_hNALLock, INFINITE);

        ret = h264_nal_parse(str_szFileUrl,m_nNalOffset,m_nNalLen,this);
        if (ret < 0)
        {
            AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
        }
    }
}
// 双击(单击)某一项，进行NAL详细分析
void CH264BSAnalyzerDlg::OnLvnItemActivateH264Nallist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    //----------------------
    POSITION ps;
    int nIndex;
    int ret = 0;

    ps=m_h264NalList.GetFirstSelectedItemPosition();
    nIndex=m_h264NalList.GetNextSelectedItem(ps);

    m_nNalOffset=m_vNalTypeVector[nIndex].data_offset;
    m_nNalLen=m_vNalTypeVector[nIndex].len;
#if 0
    SetEvent(m_hNALLock);
#else
    // 
    ret = h264_nal_parse(str_szFileUrl,m_nNalOffset,m_nNalLen,this);
    if (ret < 0)
    {
        AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
    }
#endif

    *pResult = 0;
}


void CH264BSAnalyzerDlg::OnLvnKeydownH264Nallist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    POSITION ps;
    int nIndex = 0;
    int ret = 0;

    // 不是上下光标的，不响应
    if (pLVKeyDown->wVKey != VK_UP && pLVKeyDown->wVKey != VK_DOWN)
    {
        return;
    }

    ps=m_h264NalList.GetFirstSelectedItemPosition();
    if (ps == NULL)
    {
        AfxMessageBox("No items were selected!");
        return;
    }
    else
    {
        while (ps)
        {
            nIndex=m_h264NalList.GetNextSelectedItem(ps);
        }
    }
    // i don't know how this works...
    // but it just ok
    if (pLVKeyDown->wVKey == VK_UP)
    {
        nIndex--;
    }
    else if (pLVKeyDown->wVKey == VK_DOWN)
    {
        nIndex++;
    }

    if (nIndex < 0) nIndex = 0;
    if (nIndex > m_nValTotalNum - 1) nIndex = m_nValTotalNum - 1;

    // test
#if 0
    CString aaa;
    aaa.Format("line: %d key: %x", nIndex, pLVKeyDown->wVKey);
    GetDlgItem(IDC_EDIT_SIMINFO)->SetWindowTextA(aaa);

    return;
#endif

    m_nNalOffset=m_vNalTypeVector[nIndex].data_offset;
    m_nNalLen=m_vNalTypeVector[nIndex].len;

#if 0
    SetEvent(m_hNALLock);
#else
    ret = h264_nal_parse(str_szFileUrl,m_nNalOffset,m_nNalLen,this);
    if (ret < 0)
    {
        AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
    }
#endif

    *pResult = 0;
}

void CH264BSAnalyzerDlg::OnNMCustomdrawH264Nallist(NMHDR *pNMHDR, LRESULT *pResult)
{
    //This code based on Michael Dunn's excellent article on
    //list control custom draw at http://www.codeproject.com/listctrl/lvcustomdraw.asp

    COLORREF clrNewTextColor, clrNewBkColor;
    int nItem;
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
    {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.

        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
    {
        clrNewTextColor = RGB(0,0,0);
        clrNewBkColor = RGB(255,255,255);

        nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

        CString strTemp = m_h264NalList.GetItemText(nItem,5);   // 第5列是类型，判断之
        if(strncmp(strTemp,"SLICE", 5)==0)
        {
            clrNewBkColor = RGB(0,255,255);       //青色
        }
        else if(strncmp(strTemp,"VPS", 3)==0)
        {
            clrNewBkColor = RGB(0,0,255);        //蓝色
        }
        else if(strncmp(strTemp,"SPS", 3)==0)
        {
            clrNewBkColor = RGB(255,255,0);        //黄色
        }
        else if(strncmp(strTemp,"PPS", 3)==0)
        {
            clrNewBkColor = RGB(255,153,0);        //咖啡色
        }
        else if(strncmp(strTemp,"SEI", 3)==0)
        {
            clrNewBkColor = RGB(255,66,255);       //粉红色
        }
        else if(strncmp(strTemp,"IDR", 3)==0)
        {
            clrNewBkColor = RGB(255,0,0);          //红色
        }
        else if(strncmp(strTemp,"P Slice", 7)==0)
        {
            // 只有第5列才显示这里设置的颜色
            if (pLVCD->iSubItem == 5)
                clrNewTextColor = RGB(0,0,255); // Blue
        }
        else if(strncmp(strTemp,"B Slice", 7)==0)
        {
            if (pLVCD->iSubItem == 5)
                clrNewTextColor = RGB(0,255,0); // Green
        }
        else if(strncmp(strTemp,"I Slice", 7)==0)
        {
            if (pLVCD->iSubItem == 5)
                clrNewTextColor = RGB(255,0,0); // Red
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
    }
}


// 主界面需要设置Accept Files为TRUE
void CH264BSAnalyzerDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: Add your message handler code here and/or call default
    CDialogEx::OnDropFiles(hDropInfo);

    char* pFilePathName =(char *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, (LPSTR)pFilePathName, MAX_URL_LENGTH);  // 获取拖放文件的完整文件名，最关键！
    //m_h264InputUrl.SetWindowTextA(pFilePathName);
    m_edFileUrl.SetWindowText((LPSTR)pFilePathName);
    m_strFileUrl.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);

    OnBnClickedH264InputurlOpen();
}

void CH264BSAnalyzerDlg::OnFileOpen()
{
    // TODO: Add your command handler code here

    CString strFilePath;
    char szFilter[] = "H.264 Files(*.h264;*.264)|*.h264;*.264|All Files(*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "H.264", NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("选择H.264码流文件");   // 标题
    if (fileDlg.DoModal() != IDOK)
    {
        return;
    }

    m_strFileUrl = fileDlg.GetPathName();

    m_edFileUrl.SetWindowTextA(m_strFileUrl);
}

void CH264BSAnalyzerDlg::OnHelpAbout()
{
    // TODO: Add your command handler code here
    CAboutDlg dlg;
    dlg.DoModal();
}


void CH264BSAnalyzerDlg::OnHowtoUsage()
{
    // TODO: Add your command handler code here
    char* help = "用法：\r\n"
        "1、打开文件\r\n"
        "1)使用file菜单打开；2)将文件拖到本界面；3)在文件编辑框输入文件绝对路径\r\n"
        "2、点击start，开始分析NAL\r\n"
        "3、双击某一项NAL，即可得到详细信息\r\n"
        "4、可以在show nal num下拉框选择要分析的NAL个数，也可以手动输入数值。\r\n"
        "限制：本程序仅能分析H264码流文件，其它文件无法分析\r\n"
        "本程序不能分析大型文件，勿怪\r\n";
    AfxMessageBox(help);
}

// about对话框的东东
BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    GetDlgItem(IDC_STATIC_URL)->GetWindowRect(&m_pRectLink);
    ScreenToClient(&m_pRectLink);
    GetDlgItem(IDC_STATIC_WEB)->GetWindowRect(&m_pRectLink1);
    ScreenToClient(&m_pRectLink1);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    HCURSOR hCursor;
    // TODO: Add your message handler code here and/or call default
    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y < m_pRectLink.bottom && point.y > m_pRectLink.top)
    {
        //变成手形    
        hCursor = ::LoadCursor (NULL, IDC_HAND);
        ::SetCursor(hCursor);
    }
    if (point.x > m_pRectLink1.left && point.x < m_pRectLink1.right && point.y < m_pRectLink1.bottom && point.y > m_pRectLink1.top)
    {
        hCursor = ::LoadCursor (NULL, IDC_HAND);
        ::SetCursor(hCursor);
    }
    CDialogEx::OnMouseMove(nFlags, point);
}


void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CString strLink;

    if (point.x > m_pRectLink.left && point.x < m_pRectLink.right && point.y < m_pRectLink.bottom && point.y > m_pRectLink.top)
    {
        if (nFlags == MK_LBUTTON)
        {
            GetDlgItem(IDC_STATIC_URL)->GetWindowText(strLink);
            ShellExecute(NULL, NULL, strLink, NULL, NULL, SW_NORMAL);
        }
    }
    if (point.x > m_pRectLink1.left && point.x < m_pRectLink1.right && point.y < m_pRectLink1.bottom && point.y > m_pRectLink1.top)
    {
        if (nFlags == MK_LBUTTON)
        {
            GetDlgItem(IDC_STATIC_WEB)->GetWindowText(strLink);
            ShellExecute(NULL, NULL, strLink, NULL, NULL, SW_NORMAL);
        }
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

