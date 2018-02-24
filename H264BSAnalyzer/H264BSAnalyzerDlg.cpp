
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
    m_strFileUrl.Empty();
    m_pPlayDlg = NULL;
    m_fCanPlay = FALSE;
    memset(&m_cVideoInfo, '\0', sizeof(videoinfo_t));
}

void CH264BSAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_H264_NALLIST, m_h264NalList);
    DDX_Control(pDX, IDC_EDIT_HEX, m_edHexInfo);
    DDX_Control(pDX, IDC_TREE1, m_cTree);
}

BEGIN_MESSAGE_MAP(CH264BSAnalyzerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnLvnItemActivateH264Nallist)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnNMCustomdrawH264Nallist)
    ON_COMMAND(ID_FILE_OPEN32771, &CH264BSAnalyzerDlg::OnFileOpen)
    ON_COMMAND(ID_HELP_ABOUT, &CH264BSAnalyzerDlg::OnHelpAbout)
    ON_COMMAND(ID_HOWTO_USAGE, &CH264BSAnalyzerDlg::OnHowtoUsage)
    ON_NOTIFY(LVN_KEYDOWN, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnLvnKeydownH264Nallist)
    ON_WM_SIZE()
    ON_COMMAND(ID_PLAY_PLAY, &CH264BSAnalyzerDlg::OnPlayDlg)
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

    m_nSliceIndex = 0;

    m_nValTotalNum = 0;

    m_strFileUrl.Empty();

    m_edHexInfo.SetOptions(1, 1, 1, 1);
    m_edHexInfo.SetBPR(16); // 16字节

    // 主窗口大小
    GetClientRect(&m_rectMainWnd);
    //this->GetWindowRect(&m_rectMainWnd);
    //ScreenToClient(m_rectMainWnd);

    //CRect rect;
    //GetClientRect(&m_rectMainWnd);
    //int i = rect.Width();
    //int j = rect.Height();

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

#if 0
#define AddTreeItem(item, buffer) m_cTree.InsertItem(buffer,item)


    HTREEITEM hItem = m_cTree.InsertItem("根节点NAL",TVI_ROOT);///root
    CString strTemp;
    strTemp.Format("NAL头节点nal_unit_header");
    HTREEITEM hSubItem = AddTreeItem(hItem, strTemp.GetBuffer());

    strTemp.Format("forbidden_zero_bit \t\t:0 (1 bit)");
    AddTreeItem(hSubItem, strTemp.GetBuffer());
    strTemp.Format("nal_unit_type \t\t:32 (6 bit)");
    AddTreeItem(hSubItem, strTemp.GetBuffer());
    strTemp.Format("nal_ref_idc \t\t:0 (6 bit)");
    AddTreeItem(hSubItem, strTemp.GetBuffer());
    strTemp.Format("nuh_temporal_id_plus1 \t\t:0 (3 bit)");
    AddTreeItem(hSubItem, strTemp.GetBuffer());

    strTemp.Format("VPS节点video_parameter_set_rbsp()");
    HTREEITEM hItem1 = AddTreeItem(hItem, strTemp.GetBuffer());

    strTemp.Format("header()");
    HTREEITEM hItem2 = AddTreeItem(hItem1, strTemp.GetBuffer());
    strTemp.Format("fist slice)");
    AddTreeItem(hItem2, strTemp.GetBuffer());
    strTemp.Format("no output");
    AddTreeItem(hItem2, strTemp.GetBuffer());

    strTemp.Format("data()");
    AddTreeItem(hItem1, strTemp.GetBuffer());
#endif

    // 初始化 播放 对话框
#if 0
    if (m_pPlayDlg == NULL)
    {
        m_pPlayDlg = new CPlayDlg();
        m_pPlayDlg->Create(IDD_PLAYDLG, this);
    }
#endif

    /*
    CMenu popMenu;
    CMenu *pPopup;
    popMenu.LoadMenu(IDR_MENU);

    pPopup=popMenu.GetSubMenu(1);

    pPopup->EnableMenuItem(ID_PLAY_PLAY, MF_BYCOMMAND|MF_ENABLED);
    pPopup->Detach();
    popMenu.DestroyMenu();
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
        switch (nalu->nalType)
        {
        case 0:
            strNalUnitType.Format(_T("Unspecified"));
            break;
        case 1:
            strNalUnitType.Format(_T("Coded slice of a non-IDR picture"));
            switch (nalu->sliceType)
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
        switch (nalu->nalType)
        {
        // to confirm type...
        case NAL_UNIT_CODED_SLICE_TRAIL_N:
        case NAL_UNIT_CODED_SLICE_TRAIL_R:
            strNalUnitType.Format(_T("Coded slice segment of a non-TSA, non-STSA trailing picture"));
            switch (nalu->sliceType)
            {
            case H265_SH_SLICE_TYPE_B:
                strNalInfo.Format(_T("B Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_P:
                strNalInfo.Format(_T("P Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_I:
                strNalInfo.Format(_T("I Slice #%d"), m_nSliceIndex);
                break;
            }
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_TSA_N:
        case NAL_UNIT_CODED_SLICE_TSA_R:
            strNalUnitType.Format(_T("Coded slice segment of a TSA picture"));
            switch (nalu->sliceType)
            {
            case H265_SH_SLICE_TYPE_B:
                strNalInfo.Format(_T("B Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_P:
                strNalInfo.Format(_T("P Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_I:
                strNalInfo.Format(_T("I Slice #%d"), m_nSliceIndex);
                break;
            }
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_RADL_N:
        case NAL_UNIT_CODED_SLICE_RADL_R:
            strNalUnitType.Format(_T("Coded slice segment of a TSA picture"));
            switch (nalu->sliceType)
            {
            case H265_SH_SLICE_TYPE_B:
                strNalInfo.Format(_T("B Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_P:
                strNalInfo.Format(_T("P Slice #%d"), m_nSliceIndex);
                break;
            case H265_SH_SLICE_TYPE_I:
                strNalInfo.Format(_T("I Slice #%d"), m_nSliceIndex);
                break;
            }
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_IDR_W_RADL:
        case NAL_UNIT_CODED_SLICE_IDR_N_LP:
            strNalUnitType.Format(_T("Coded slice of an IDR picture"));
            strNalInfo.Format(_T("IDR #%d"), m_nSliceIndex);
            m_nSliceIndex++;
            break;
        case NAL_UNIT_CODED_SLICE_CRA:
            strNalUnitType.Format(_T("Coded slice segment of a CRA picture"));
            strNalInfo.Format(_T("CRA #%d"), m_nSliceIndex);
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
    strOffset.Format(_T("%08X"), nalu->offset);
    // 长度
    strNalLen.Format(_T("%d"),nalu->len);
    // 起始码
    strStartCode.Format(_T("%s"), nalu->startcodeBuffer);

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

    if(m_strFileUrl.IsEmpty()==TRUE)
    {
        AfxMessageBox(_T("文件路径为空，请打开文件！！"));
        return;
    }
    CString strTemp;
    strTemp.Format("%s - %s", m_strFileUrl, APP_NAM);
    this->SetWindowText(strTemp);
    GetDlgItem(IDC_EDIT_SIMINFO)->SetWindowText("");

    m_fCanPlay = FALSE;
    SetEvent(m_hFileLock);
}

UINT CH264BSAnalyzerDlg::ThreadFuncReadFile(LPVOID lpvoid)
{
    CH264BSAnalyzerDlg* dlg = static_cast<CH264BSAnalyzerDlg*>(lpvoid);
    dlg->ReadFile();
    return 0;
}

void CH264BSAnalyzerDlg::ReadFile(void)
{
    CString strFilePath;
    CString strSimpleInfo;
    CString strProfileInfo;
    CString strLevelInfo;
    CString strTierInfo;
    CString strVideoFormat;
    CString strBitDepth;
    CString strMaxNalNum;
    int nMaxNalNum = -1;
    SPSInfo_t sps = {0};
    PPSInfo_t pps = {0};

    strTierInfo.Empty();

    while (true)
    {
        WaitForSingleObject(m_hFileLock, INFINITE);

        m_cParser.probeNALU(m_vNalTypeVector, nMaxNalNum);

        m_nValTotalNum = m_vNalTypeVector.size();

        for (int i = 0; i < m_nValTotalNum; i++)
        {
            ShowNLInfo(&m_vNalTypeVector[i]);
        }

        m_cParser.getVideoInfo(&m_cVideoInfo);
        // H.265
        if (m_cVideoInfo.type)
        {
            // profile类型
            switch (m_cVideoInfo.profile_idc)
            {
            case PROFILE_NONE:
                strProfileInfo.Format(_T("None"));
                break;
            case PROFILE_MAIN:
                strProfileInfo.Format(_T("Main"));
                break;
            case PROFILE_MAIN10:
                strProfileInfo.Format(_T("Main10"));
                break;
            case PROFILE_MAINSTILLPICTURE:
                strProfileInfo.Format(_T("Main Still Picture"));
                break;
            case PROFILE_MAINREXT:
                strProfileInfo.Format(_T("Main RExt"));
                break;
            case PROFILE_HIGHTHROUGHPUTREXT:
                strProfileInfo.Format(_T("High Throughput RExt"));
                break;
            default:
                strProfileInfo.Format(_T("Unkown"));
                break;
            }
            switch (m_cVideoInfo.level_idc)
            {
            case LEVEL_NONE:
                strLevelInfo.Format(_T("none(%d)"), LEVEL_NONE);
                break;
            case LEVEL1:
                strLevelInfo.Format(_T("1(%d)"), LEVEL1);
                break;
            case LEVEL2:
                strLevelInfo.Format(_T("2(%d)"), LEVEL2);
                break;
            case LEVEL2_1:
                strLevelInfo.Format(_T("2.1(%d)"), LEVEL2_1);
                break;
            case LEVEL3:
                strLevelInfo.Format(_T("3(%d)"), LEVEL3);
                break;
            case LEVEL3_1:
                strLevelInfo.Format(_T("3.1(%d)"), LEVEL3_1);
                break;
            case LEVEL4:
                strLevelInfo.Format(_T("4(%d)"), LEVEL4);
                break;
            case LEVEL4_1:
                strLevelInfo.Format(_T("4.1(%d)"), LEVEL4_1);
                break;
            case LEVEL5:
                strLevelInfo.Format(_T("5(%d)"), LEVEL5);
                break;
            case LEVEL5_1:
                strLevelInfo.Format(_T("5.1(%d)"), LEVEL5_1);
                break;
            case LEVEL5_2:
                strLevelInfo.Format(_T("5.2(%d)"), LEVEL5_2);
                break;
            case LEVEL6:
                strLevelInfo.Format(_T("6(%d)"), LEVEL6);
                break;
            case LEVEL6_1:
                strLevelInfo.Format(_T("6.1(%d)"), LEVEL6_1);
                break;
            case LEVEL6_2:
                strLevelInfo.Format(_T("6.2(%d)"), LEVEL6_2);
                break;
            case LEVEL8_5:
                strLevelInfo.Format(_T("8.5(%d)"), LEVEL8_5);
                break;
            default:
                strLevelInfo.Format(_T("Unkown"));
                break;
            }
            switch (m_cVideoInfo.tier_idc)
            {
            case 1:
                strTierInfo.Format(_T("Tier High"));
                break;
            case 0:
            default:
                strTierInfo.Format(_T("Tier Main"));
                break;
            }
        }
        else // h264
        {
            // profile类型
            switch (m_cVideoInfo.profile_idc)
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
            strTierInfo.Empty();
            strLevelInfo.Format(_T("%d"), m_cVideoInfo.level_idc);
        }
        // common
        // bit depth
        strBitDepth.Format(_T("Luma bit: %d Chroma bit: %d"), m_cVideoInfo.bit_depth_luma, m_cVideoInfo.bit_depth_chroma);

        // chroma format
        switch (m_cVideoInfo.chroma_format_idc)
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

        /*
        "Video Format: xxx\r\n"
        */
        strSimpleInfo.Format(
            "%s File Information\r\n\r\n"
            "Picture Size \t: %dx%d\r\n"
            "  - Cropping Left \t: %d\r\n"
            "  - Cropping Right \t: %d\r\n"
            "  - Cropping Top \t: %d\r\n"
            "  - Cropping Bottom : %d\r\n"
            "Video Format \t: %s %s\r\n"
            "Stream Type \t: %s Profile @ Level %s %s\r\n"
            "Encoding Type \t: %s\r\n"
            "Max fps \t\t: %.03f\r\n"
            "Frame Count \t: %d\r\n",
            m_cVideoInfo.type ? "H.265/HEVC" : "H.264/AVC",
            m_cVideoInfo.width, m_cVideoInfo.height,
            m_cVideoInfo.crop_left, m_cVideoInfo.crop_right,
            m_cVideoInfo.crop_top, m_cVideoInfo.crop_bottom,
            strVideoFormat, strBitDepth, 
            strProfileInfo, strLevelInfo, strTierInfo,
            m_cVideoInfo.encoding_type ? "CABAC" : "CAVLC",
            m_cVideoInfo.max_framerate, m_nSliceIndex
            );
        GetDlgItem(IDC_EDIT_SIMINFO)->SetWindowText(strSimpleInfo);
        m_fCanPlay = TRUE;
    }
}

UINT CH264BSAnalyzerDlg::ThreadFuncPaseNal(LPVOID lpvoid)
{
    CH264BSAnalyzerDlg* dlg = static_cast<CH264BSAnalyzerDlg*>(lpvoid);
    //dlg->PaseNal();
    return 0;
}

/*
void CH264BSAnalyzerDlg::PaseNal(void)
{
    int ret = 0;
    while (true)
    {
        WaitForSingleObject(m_hNALLock, INFINITE);

        ret = m_cParser.parseNALU(m_strFileUrl.GetBuffer(),m_nNalOffset,m_nNalLen,this);
        if (ret < 0)
        {
            AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
        }
    }
}
*/
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

#if 0
    SetEvent(m_hNALLock);
#else
    char* nalData = NULL;
    char* nalInfo = NULL;
    ret = m_cParser.parseNALU(m_vNalTypeVector[nIndex], &nalData, &nalInfo);
    if (ret < 0)
    {
        AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
    }

    // 显示十六进制
    m_edHexInfo.SetData((LPBYTE)nalData, m_vNalTypeVector[nIndex].len);
    ::SendMessage(GetDlgItem(IDC_EDIT_HEX)->m_hWnd,WM_KILLFOCUS,-1,0); // 不要控件焦点
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

#if 0
    SetEvent(m_hNALLock);
#else
    char* nalData = NULL;
    char* nalInfo = NULL;
    ret = m_cParser.parseNALU(m_vNalTypeVector[nIndex], &nalData, &nalInfo);
    if (ret < 0)
    {
        AfxMessageBox("解析NAL时出错，可能是文件读取出错。");
    }

    // 显示十六进制
    m_edHexInfo.SetData((LPBYTE)nalData, m_vNalTypeVector[nIndex].len);
    ::SendMessage(GetDlgItem(IDC_EDIT_HEX)->m_hWnd,WM_KILLFOCUS,-1,0); // 不要控件焦点
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
            clrNewBkColor = RGB(255,0,255);        //紫色
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
            clrNewBkColor = RGB(128,128,192);       //灰色
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
    CDialogEx::OnDropFiles(hDropInfo);

    char* pFilePathName =(char *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, (LPSTR)pFilePathName, MAX_URL_LENGTH);  // 获取拖放文件的完整文件名，最关键！
    m_strFileUrl.Format(_T("%s"), pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);

    FILE* fp = fopen(m_strFileUrl.GetBuffer(), "r+b");
    if (fp == NULL)
    {
        AfxMessageBox("Error open file.");
        return;
    }
    fclose(fp);

    int ret = m_cParser.init(m_strFileUrl.GetBuffer(), &m_cTree);
    if (ret < 0)
    {
        MessageBox("Maybe not H264 or H265 file?");
        return;
    }
    OnBnClickedH264InputurlOpen();
}

void CH264BSAnalyzerDlg::OnFileOpen()
{
    CString strFilePath;
    char szFilter[] = "H.264 or H.265 Files(*.h264;*.264;*.h265;*.265)|*.h264;*.264;*.h265;*.265|All Files(*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "H.264", NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFilter);
    fileDlg.GetOFN().lpstrTitle = _T("选择H.264或H.265码流文件");   // 标题
    if (fileDlg.DoModal() != IDOK)
    {
        return;
    }

    m_strFileUrl = fileDlg.GetPathName();

    FILE* fp = fopen(m_strFileUrl.GetBuffer(), "r+b");
    if (fp == NULL)
    {
        AfxMessageBox("Error open file.");
        return;
    }
    fclose(fp);

    int ret = m_cParser.init(m_strFileUrl.GetBuffer(), &m_cTree);
    if (ret < 0)
    {
        MessageBox("Maybe not H264 or H265 file?");
        return;
    }

    OnBnClickedH264InputurlOpen();
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
        "1)使用file菜单打开；2)将文件拖到本界面\r\n"
        "2、程序自动解析，较大文件耗时较大\r\n"
        "3、双击某一项NAL，即可得到详细信息\r\n"
        "4、打开文件后，使用Play可进行播放、暂停、停止、保存图片/视频等操作\r\n"
        "限制：本程序仅能分析H264/H265码流文件，其它文件无法分析\r\n"
        "本程序分析大型文件较慢，也可能会有问题\r\n";
    MessageBox(help);
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

/**
主窗口使用GetClientRect是因为需要与本函数的cx、cy计算缩放比例。
控件使用GetWindowRect和ScreenToClient是为了得到相对的坐标。
note：
固定的数值是测试得到的经验值，无理论依据
*/
void CH264BSAnalyzerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // 非法值，什么也不做
    if (cx <= 0 || cy <= 0) return;

    // 先判断窗口句柄合法性
    if ( !GetDlgItem(IDC_H264_NALLIST) || !GetDlgItem(IDC_STATIC) ||
        !GetDlgItem(IDC_EDIT_HEX) || !GetDlgItem(IDC_EDIT_SIMINFO) ||
        !GetDlgItem(IDC_TREE1) )
        return;
    CRect rectList, rectHex, rectTxt, rectInfo, rectTree, rectMainWnd;
    CWnd *pWnd = NULL;
    GetDlgItem(IDC_H264_NALLIST)->GetWindowRect(&rectList);
    ScreenToClient(rectList);
    GetDlgItem(IDC_STATIC)->GetWindowRect(&rectTxt);
    ScreenToClient(rectTxt);
    GetDlgItem(IDC_EDIT_HEX)->GetWindowRect(&rectHex);
    ScreenToClient(rectHex);
    GetDlgItem(IDC_EDIT_SIMINFO)->GetWindowRect(&rectInfo);
    ScreenToClient(rectInfo);
    GetDlgItem(IDC_TREE1)->GetWindowRect(&rectTree);
    ScreenToClient(rectTree);

#if 0
    int a = rectList.Width();
    int b = rectList.Height();
    int a1 = rectList.left;
    int b1 = rectList.top;

    int c1 = rectTxt.Width();
    int d1 = rectTxt.Height();

    int c = rectHex.Width();
    int d = rectHex.Height();
    int e = rectInfo.Width();
    int f = rectInfo.Height();
    int g = rectTree.Width();
    int h = rectTree.Height();

    int i = m_rectMainWnd.Width(); // 16
    int j = m_rectMainWnd.Height(); // 58
#endif

    float fXRatio = (float)cx / (float)(m_rectMainWnd.Width());
    float fYRatio = (float)cy / (float)(m_rectMainWnd.Height());

    int nNewWidth = 0;
    int nNewHeight = 0;

    // 列表框
    pWnd = GetDlgItem(IDC_H264_NALLIST);
    nNewWidth = (int)(fXRatio * (float)rectList.Width());
    nNewHeight = (int)(fYRatio * (float)rectList.Height());
    pWnd->MoveWindow(rectList.left, rectList.top, nNewWidth, nNewHeight);
    pWnd->Invalidate();
    pWnd->UpdateData();
    pWnd->GetWindowRect(&rectList);
    ScreenToClient(rectList);

    // 列表框里面的
    nNewWidth = (int)(m_h264NalList.GetColumnWidth(4) * fXRatio);
    m_h264NalList.SetColumnWidth(4, nNewWidth);
    nNewWidth = (int)(m_h264NalList.GetColumnWidth(5) * fXRatio);
    m_h264NalList.SetColumnWidth(5, nNewWidth);
    ///////////////////////

    // "Hex View" 文本
    pWnd = GetDlgItem(IDC_STATIC);
    pWnd->GetWindowRect(&rectTxt);
    ScreenToClient(rectTxt);
    pWnd->MoveWindow(rectTxt.left, rectList.Height(), rectTxt.Width(), rectTxt.Height());
    
    pWnd->Invalidate();
    pWnd->SetWindowText("Hex View");
    pWnd->UpdateData();
    pWnd->GetWindowRect(&rectTxt);
    ScreenToClient(rectTxt);

    // 十六进制框
    pWnd = GetDlgItem(IDC_EDIT_HEX);
    nNewWidth = (int)(fXRatio * (float)rectHex.Width());
    nNewHeight = (int)(fYRatio * (float)rectHex.Height());
    pWnd->MoveWindow(rectHex.left, rectList.Height()+rectTxt.Height(), nNewWidth, cy - rectList.Height() - 18);
    pWnd->Invalidate();
    pWnd->UpdateData();
    pWnd->GetWindowRect(&rectHex);
    ScreenToClient(rectHex);

    // 信息框
    pWnd = GetDlgItem(IDC_EDIT_SIMINFO);
    nNewWidth = (int)(fXRatio * (float)rectInfo.Width());
    nNewHeight = (int)(fYRatio * (float)rectInfo.Height());
    pWnd->MoveWindow(rectList.Width()+6, rectInfo.top, cx - rectList.Width() - 8, nNewHeight);
    pWnd->Invalidate();
    pWnd->UpdateData();
    pWnd->GetWindowRect(&rectInfo);
    ScreenToClient(rectInfo);

    // 树形控件框
    pWnd = GetDlgItem(IDC_TREE1);
    nNewWidth = (int)(fXRatio * (float)rectTree.Width());
    nNewHeight = (int)(fYRatio * (float)rectTree.Height());
    pWnd->MoveWindow(rectList.Width()+6, rectInfo.Height()+5, cx - rectList.Width() - 8, cy - rectInfo.Height() - 8);
    pWnd->Invalidate();
    pWnd->UpdateData();

    // 更新当前主窗口大小
    GetClientRect(&m_rectMainWnd);
}

void CH264BSAnalyzerDlg::OnPlayDlg()
{
    if (!m_fCanPlay)
    {
        MessageBox("Parsing NALU, wait a momnent...");
        return;
    }
    // 非模态对话框
    if (m_pPlayDlg == NULL)
    {
        m_pPlayDlg = new CPlayDlg();
        m_pPlayDlg->Create(IDD_PLAYDLG, this);
    }
    // 把主窗口打开的文件信息传到子窗口
    int ret = m_pPlayDlg->SetVideoInfo(m_strFileUrl, m_cVideoInfo.width, m_cVideoInfo.height, m_nSliceIndex, m_cVideoInfo.max_framerate);
    if (ret < 0) return;

    m_pPlayDlg->ShowWindow(SW_SHOW);

    m_pPlayDlg->ShowFirstFrame(); // 注：在调用ShowWindow后再显示第一帧图像
}
