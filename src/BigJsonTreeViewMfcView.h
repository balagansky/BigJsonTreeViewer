
// BigJsonTreeViewMfcView.h : interface of the CBigJsonTreeViewMfcView class
//

#pragma once

#include "ViewTree.h"

class CBigJsonTreeViewMfcView : public CView
{
protected: // create from serialization only
	CBigJsonTreeViewMfcView() noexcept;
	DECLARE_DYNCREATE(CBigJsonTreeViewMfcView)

// Attributes
public:
	CBigJsonTreeViewMfcDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView) override;

	void AdjustLayout();

// Implementation
public:
	virtual ~CBigJsonTreeViewMfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CViewTree m_wndTreeView;

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BigJsonTreeViewMfcView.cpp
inline CBigJsonTreeViewMfcDoc* CBigJsonTreeViewMfcView::GetDocument() const
   { return reinterpret_cast<CBigJsonTreeViewMfcDoc*>(m_pDocument); }
#endif

