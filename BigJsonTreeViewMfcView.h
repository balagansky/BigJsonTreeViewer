
// BigJsonTreeViewMfcView.h : interface of the CBigJsonTreeViewMfcView class
//

#pragma once


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
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CBigJsonTreeViewMfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

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

