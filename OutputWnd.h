
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

class COutputList : public CListBox
{
// Construction
public:
	COutputList() noexcept;

// Implementation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();
	afx_msg void OnLbnSelchange();

	DECLARE_MESSAGE_MAP()
public:
};

class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd() noexcept;

	void UpdateFonts();

// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputInfo;
	COutputList m_wndOutputDebug;
	COutputList m_wndOutputFind;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

