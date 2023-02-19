
#include "pch.h"
#include "framework.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#include "Butter/LoggerImpl.h"
#include "Butter/SearchResults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	auto fCreateTab = [&](CListBox &listBox, int strId, DWORD extraStyle = 0) {
		static int tabId = 2;
		if (!listBox.Create(dwStyle | extraStyle, rectDummy, &m_wndTabs, tabId++))
			return false;
		listBox.SetFont(&afxGlobalData.fontRegular);

		CString strTabName;
		BOOL bNameValid;

		// Attach list windows to tab:
		bNameValid = strTabName.LoadString(strId);
		ASSERT(bNameValid);
		m_wndTabs.AddTab(&listBox, strTabName);

		return true;
	};

	if (!fCreateTab(m_wndOutputInfo, IDS_BUILD_TAB) ||
		!fCreateTab(m_wndOutputFind, IDS_FIND_TAB, LBS_NOTIFY))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	LoggerHooks::gWriteLineFn = [this](const char* msg) {
		m_wndOutputInfo.AddString(msg);
		m_wndOutputInfo.SetTopIndex(m_wndOutputInfo.GetCount() - 1);
		OutputDebugString(msg);
		OutputDebugString("\n");
	};

	SearchResults::SetCallbacks({
		.clear = [&] {m_wndOutputFind.ResetContent();},
		.focus = [&] {
			m_wndTabs.SetActiveTab(m_wndTabs.GetTabFromHwnd(m_wndOutputFind));
		},
		.addLine = [&](const char* str) {
			auto index = m_wndOutputFind.AddString(str);
			m_wndOutputFind.SetTopIndex(m_wndOutputInfo.GetCount() - 1);
			return index;
		}
		});

	m_wndOutputInfo.AddString(_T("Welcome to Big JSON Tree Viewer!"));

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = std::max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputInfo.SetFont(&afxGlobalData.fontRegular);
	/*m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);*/
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() noexcept
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	//ON_COMMAND(LBN_DBLCLK, OnFindResultClick)
	ON_WM_WINDOWPOSCHANGING()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &COutputList::OnLbnSelchange)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("Copy output"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

#include "Butter/Log.h"
void COutputList::OnLbnSelchange()
{
	SearchResults::SelectResultLine(GetCurSel());
}
