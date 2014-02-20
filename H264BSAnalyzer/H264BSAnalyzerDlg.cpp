
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

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CH264BSAnalyzerDlg dialog




CH264BSAnalyzerDlg::CH264BSAnalyzerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CH264BSAnalyzerDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CH264BSAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_H264_INPUTURL, m_h264InputUrl);
    DDX_Control(pDX, IDC_H264_NALINFO, m_h264NalInfo);
    DDX_Control(pDX, IDC_H264_NALLIST, m_h264NalList);
}

BEGIN_MESSAGE_MAP(CH264BSAnalyzerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_H264_INPUTURL_OPEN, &CH264BSAnalyzerDlg::OnBnClickedH264InputurlOpen)
    ON_WM_DROPFILES()
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnLvnItemActivateH264Nallist)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_H264_NALLIST, &CH264BSAnalyzerDlg::OnNMCustomdrawH264Nallist)
END_MESSAGE_MAP()


void CH264BSAnalyzerDlg::SystemClear()
{
    m_vNalInfoVector.clear();
    m_h264NalList.DeleteAllItems();
    m_nNalIndex = 0;
}

//添加一条记录
//每个字段的含义：类型，数据大小，时间戳，streamid，data的第一个字节
//data_lenth是包含起始码的NAL长度
//len是不包含起始码的NAL长度
int CH264BSAnalyzerDlg::AppendNLInfo(int nal_reference_idc,int nal_unit_type,int len,int data_lenth,int data_offset){
	//如果选择了“最多输出5000条”，判断是否超过5000条
	//if(m_vh264nallistmaxnum.GetCheck()==1&&nl_index>5000){
	//	return 0;
	//}

	CString temp_index,temp_nal_reference_idc,temp_nal_unit_type,temp_len;
	int nIndex=0;
	switch(nal_unit_type){
	case 1:temp_nal_unit_type.Format("SLICE");break;
	case 2:temp_nal_unit_type.Format("DPA");break;
	case 3:temp_nal_unit_type.Format("DPB");break;
	case 4:temp_nal_unit_type.Format("DPC");break;
	case 5:temp_nal_unit_type.Format("IDR_SLICE");break;
	case 6:temp_nal_unit_type.Format("SEI");break;
	case 7:temp_nal_unit_type.Format("SPS");break;
	case 8:temp_nal_unit_type.Format("PPS");break;
	case 9:temp_nal_unit_type.Format("AUD");break;
	case 10:temp_nal_unit_type.Format("END_SEQUENCE");break;
	case 11:temp_nal_unit_type.Format("END_STREAM");break;
	case 12:temp_nal_unit_type.Format("FILLER_DATA");break;
	case 13:temp_nal_unit_type.Format("SPS_EXT");break;
	case 19:temp_nal_unit_type.Format("AUXILIARY_SLICE");break;
	default:temp_nal_unit_type.Format("其他");break;
	}
	temp_index.Format("%d",m_nNalIndex);
	temp_nal_reference_idc.Format("%d",nal_reference_idc);
	temp_len.Format("%d",len);
	//获取当前记录条数
	nIndex=m_h264NalList.GetItemCount();
	//“行”数据结构
	LV_ITEM lvitem;
	lvitem.mask=LVIF_TEXT;
	lvitem.iItem=nIndex;
	lvitem.iSubItem=0;
	//注：vframe_index不可以直接赋值！
	//务必使用f_index执行Format!再赋值！
	lvitem.pszText=(char *)(LPCTSTR)temp_index;
	//------------------------
	//这个vector记录了nal的位置信息
	//使用它我们可以获取到NAL的详细信息
	//我们要存储包含起始码的长度
	//起始码原本不是NAL的一部分
	NALInfo nalinfo;
	nalinfo.data_lenth=data_lenth;
	nalinfo.data_offset=data_offset;
	m_vNalInfoVector.push_back(nalinfo);
	//------------------------
	m_h264NalList.InsertItem(&lvitem);
	m_h264NalList.SetItemText(nIndex,5,temp_nal_reference_idc);
	m_h264NalList.SetItemText(nIndex,3,temp_nal_unit_type);
	m_h264NalList.SetItemText(nIndex,2,temp_len);

	m_nNalIndex++;
	return TRUE;
}


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
    //视频
    m_h264NalList.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
    m_h264NalList.SetExtendedStyle(dwExStyle);

    m_h264NalList.InsertColumn(0,"No.",LVCFMT_CENTER,40,0);
    m_h264NalList.InsertColumn(1,"Offset",LVCFMT_CENTER,60,0);
    m_h264NalList.InsertColumn(2,"Len",LVCFMT_CENTER,60,0);
    m_h264NalList.InsertColumn(3,"NAL Type",LVCFMT_CENTER,150,0);
    m_h264NalList.InsertColumn(4,"Info",LVCFMT_CENTER,50,0);
    m_h264NalList.InsertColumn(5,"nal_ref_idc",LVCFMT_CENTER,100,0);
    //---------------------
    //m_h264NalListmaxnum.SetCheck(1);
    m_nNalIndex = 0;
    //------------
    m_h264InputUrl.EnableFileBrowseButton(NULL,
            "H.264 Files (*.264,*.h264)|*.264;*.h264|All Files (*.*)|*.*||");
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


// 打开H264码流文件
void CH264BSAnalyzerDlg::OnBnClickedH264InputurlOpen()
{
    // TODO: Add your control notification handler code here
    SystemClear();
    CString strFilePath;
    m_h264InputUrl.GetWindowText(strFilePath);
    if(strFilePath.IsEmpty()==TRUE)
    {
        AfxMessageBox("文件路径为空，请输入H.264文件！");
        return;
    }
    strcpy(m_strFileUrl,strFilePath);
    h264_nal_parse(this,m_strFileUrl);
}

// 主界面需要设置Accept Files为TRUE
void CH264BSAnalyzerDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: Add your message handler code here and/or call default
    CDialogEx::OnDropFiles(hDropInfo);

    char* pFilePathName =(char *)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName,MAX_URL_LENGTH);  // 获取拖放文件的完整文件名，最关键！
    m_h264InputUrl.SetWindowTextA(pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);
}

// 单击某一项，进行NAL详细分析
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
	//----------------------
	int data_offset,data_lenth;
	data_offset=m_vNalInfoVector[nIndex].data_offset;
	data_lenth=m_vNalInfoVector[nIndex].data_lenth;

    // 
	ret = probe_nal_unit(m_strFileUrl,data_offset,data_lenth,this);
    if (ret < 0)
    {
        AfxMessageBox("解析NAL时出错");
    }

	//----------------------
	//获得选中行的序号
	//CString str;
	//str=m_vh264nallist.GetItemText(nIndex,0);
	//AfxMessageBox(str);
	//----------------------
    *pResult = 0;
}


void CH264BSAnalyzerDlg::OnNMCustomdrawH264Nallist(NMHDR *pNMHDR, LRESULT *pResult)
{
    //This code based on Michael Dunn's excellent article on
    //list control custom draw at http://www.codeproject.com/listctrl/lvcustomdraw.asp

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

        COLORREF clrNewTextColor, clrNewBkColor;

        int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

        CString strTemp = m_h264NalList.GetItemText(nItem,3);   // 第3列是类型，判断之
        if(strcmp(strTemp,"SLICE")==0){
            clrNewTextColor = RGB(0,0,0);        //Set the text 
            clrNewBkColor = RGB(0,255,255);        //青色
        }
        else if(strcmp(strTemp,"SPS")==0){
            clrNewTextColor = RGB(0,0,0);        //text 
            clrNewBkColor = RGB(255,255,0);        //黄色
        }
        else if(strcmp(strTemp,"PPS")==0){
            clrNewTextColor = RGB(0,0,0);        //text
            clrNewBkColor = RGB(255,153,0);        //咖啡色
        }else if(strcmp(strTemp,"SEI")==0){
            clrNewTextColor = RGB(0,0,0);        //text
            clrNewBkColor = RGB(255,66,255);            //粉红色
        }else if(strcmp(strTemp,"IDR_SLICE")==0){
            clrNewTextColor = RGB(0,0,0);        //text
            clrNewBkColor = RGB(255,0,0);            //红色
        }else{
            clrNewTextColor = RGB(0,0,0);        //text
            clrNewBkColor = RGB(255,255,255);            //白色
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;


        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;

    }
}
