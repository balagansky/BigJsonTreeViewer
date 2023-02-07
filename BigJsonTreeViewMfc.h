
// BigJsonTreeViewMfc.h : main header file for the BigJsonTreeViewMfc application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CBigJsonTreeViewMfcApp:
// See BigJsonTreeViewMfc.cpp for the implementation of this class
//

class CBigJsonTreeViewMfcApp : public CWinAppEx
{
public:
	CBigJsonTreeViewMfcApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();

	void DisableUi(CCmdUI *pCmdUI) { pCmdUI->Enable( FALSE ); }

	DECLARE_MESSAGE_MAP()
};

extern CBigJsonTreeViewMfcApp theApp;
