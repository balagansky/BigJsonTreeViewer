// FindDialog.cpp : implementation file
//

#include "pch.h"
#include "BigJsonTreeViewMfc.h"
#include "afxdialogex.h"
#include "FindDialog.h"


// FindDialog dialog

IMPLEMENT_DYNAMIC(FindDialog, CDialogEx)

FindDialog::FindDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FIND_DIALOG, pParent)
	, m_SearchString(_T(""))
{

}

FindDialog::~FindDialog()
{
}

void FindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SEARCH_STRING, m_SearchString);
}


BEGIN_MESSAGE_MAP(FindDialog, CDialogEx)
	ON_EN_CHANGE(IDC_SEARCH_STRING, &FindDialog::OnEnChangeSearchString)
	ON_BN_CLICKED(IDOK, &FindDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// FindDialog message handlers


BOOL FindDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_SEARCH_STRING)->SetFocus();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
}

#include "Butter/Log.h"

void FindDialog::OnEnChangeSearchString()
{
	GetDlgItem(IDOK)->EnableWindow((BOOL)
		GetDlgItem(ID_SEARCH_STRING)->GetWindowTextLengthA());
}


void FindDialog::OnBnClickedOk()
{
	m_Confirmed = true;

	CDialogEx::OnOK();
}
