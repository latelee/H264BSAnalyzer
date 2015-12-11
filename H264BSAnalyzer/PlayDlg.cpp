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

}

CPlayDlg::~CPlayDlg()
{
}

void CPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlayDlg message handlers
