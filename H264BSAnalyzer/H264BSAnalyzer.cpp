
// H264BSAnalyzer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "H264BSAnalyzer.h"
#include "H264BSAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CH264BSAnalyzerApp

BEGIN_MESSAGE_MAP(CH264BSAnalyzerApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CH264BSAnalyzerApp construction

CH264BSAnalyzerApp::CH264BSAnalyzerApp()
{
    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CH264BSAnalyzerApp object

CH264BSAnalyzerApp theApp;


// CH264BSAnalyzerApp initialization

BOOL CH264BSAnalyzerApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    GdiplusStartupInput gdiplusStartupInput;
    //ULONG_PTR gdiplusToken;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    CWinApp::InitInstance();


    AfxEnableControlContainer();

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    CShellManager *pShellManager = new CShellManager;

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    CH264BSAnalyzerDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Delete the shell manager created above.
    if (pShellManager != NULL)
    {
        delete pShellManager;
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

int CH264BSAnalyzerApp::ExitInstance()
{
    // TODO: Add your specialized code here and/or call the base class
    GdiplusShutdown(m_gdiplusToken); // ??

    return CWinApp::ExitInstance();
}