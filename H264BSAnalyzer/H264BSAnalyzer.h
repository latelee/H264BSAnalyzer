
// H264BSAnalyzer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CH264BSAnalyzerApp:
// See H264BSAnalyzer.cpp for the implementation of this class
//

class CH264BSAnalyzerApp : public CWinApp
{
public:
	CH264BSAnalyzerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CH264BSAnalyzerApp theApp;