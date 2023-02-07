
// BigJsonTreeViewMfcView.cpp : implementation of the CBigJsonTreeViewMfcView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "BigJsonTreeViewMfc.h"
#endif

#include "BigJsonTreeViewMfcDoc.h"
#include "BigJsonTreeViewMfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBigJsonTreeViewMfcView

IMPLEMENT_DYNCREATE(CBigJsonTreeViewMfcView, CView)

BEGIN_MESSAGE_MAP(CBigJsonTreeViewMfcView, CView)
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CBigJsonTreeViewMfcView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CBigJsonTreeViewMfcView construction/destruction

CBigJsonTreeViewMfcView::CBigJsonTreeViewMfcView() noexcept
{
	// TODO: add construction code here

}

CBigJsonTreeViewMfcView::~CBigJsonTreeViewMfcView()
{
}

BOOL CBigJsonTreeViewMfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CBigJsonTreeViewMfcView drawing

void CBigJsonTreeViewMfcView::OnDraw(CDC* /*pDC*/)
{
	CBigJsonTreeViewMfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CBigJsonTreeViewMfcView printing


void CBigJsonTreeViewMfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CBigJsonTreeViewMfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBigJsonTreeViewMfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBigJsonTreeViewMfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CBigJsonTreeViewMfcView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CBigJsonTreeViewMfcView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CBigJsonTreeViewMfcView diagnostics

#ifdef _DEBUG
void CBigJsonTreeViewMfcView::AssertValid() const
{
	CView::AssertValid();
}

void CBigJsonTreeViewMfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBigJsonTreeViewMfcDoc* CBigJsonTreeViewMfcView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBigJsonTreeViewMfcDoc)));
	return (CBigJsonTreeViewMfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CBigJsonTreeViewMfcView message handlers
