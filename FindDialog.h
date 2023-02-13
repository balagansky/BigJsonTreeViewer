#pragma once
#include "afxdialogex.h"


// FindDialog dialog

class FindDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FindDialog)

public:
	FindDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~FindDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIND_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_SearchString;
	bool m_Confirmed = false;

	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeSearchString();
	afx_msg void OnBnClickedOk();
};
