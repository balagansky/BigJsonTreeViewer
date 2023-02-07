
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

#include "Bread/OpenFileManager.h"
#include "Butter/JsonTreeViewController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBigJsonTreeViewMfcView

IMPLEMENT_DYNCREATE(CBigJsonTreeViewMfcView, CView)

BEGIN_MESSAGE_MAP(CBigJsonTreeViewMfcView, CView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
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
	gJsonTreeViewController.RemoveTreeCtrl(m_wndTreeView);
}

BOOL CBigJsonTreeViewMfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CBigJsonTreeViewMfcView drawing

void CBigJsonTreeViewMfcView::OnDraw(CDC* pDC)
{
	CView::OnDraw(pDC);

	CBigJsonTreeViewMfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndTreeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CBigJsonTreeViewMfcView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndTreeView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

// CBigJsonTreeViewMfcView printing


void CBigJsonTreeViewMfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

int CBigJsonTreeViewMfcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tree view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndTreeView.Create(dwViewStyle, rectDummy, this, 12))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	AdjustLayout();

	return 0;
}

void CBigJsonTreeViewMfcView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTreeView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + /*cyTlb +*/ 1, rectClient.Width() - 2, rectClient.Height() /*- cyTlb*/ - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CBigJsonTreeViewMfcView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndTreeView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CBigJsonTreeViewMfcView::OnActivateView(BOOL bActivate, CView* pActivateView,
	CView* pDeactiveView)
{
	gJsonTreeViewController.SetTreeCtrlForFile(
		std::string(GetDocument()->GetPathName()), m_wndTreeView);
	gOpenFileManager.SetActiveFile(std::string(GetDocument()->GetPathName()));
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
