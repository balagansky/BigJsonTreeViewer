
// BigJsonTreeViewMfcDoc.cpp : implementation of the CBigJsonTreeViewMfcDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "BigJsonTreeViewMfc.h"
#endif

#include "BigJsonTreeViewMfcDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBigJsonTreeViewMfcDoc

IMPLEMENT_DYNCREATE(CBigJsonTreeViewMfcDoc, CDocument)

BEGIN_MESSAGE_MAP(CBigJsonTreeViewMfcDoc, CDocument)
END_MESSAGE_MAP()


// CBigJsonTreeViewMfcDoc construction/destruction

CBigJsonTreeViewMfcDoc::CBigJsonTreeViewMfcDoc() noexcept
{
	// TODO: add one-time construction code here

}

CBigJsonTreeViewMfcDoc::~CBigJsonTreeViewMfcDoc()
{
}

#include "Butter/Log.h"

BOOL CBigJsonTreeViewMfcDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


BOOL CBigJsonTreeViewMfcDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	Log::Info(std::string() + "Opened " + lpszPathName);

	return TRUE;
}

// CBigJsonTreeViewMfcDoc serialization

void CBigJsonTreeViewMfcDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CBigJsonTreeViewMfcDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CBigJsonTreeViewMfcDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CBigJsonTreeViewMfcDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CBigJsonTreeViewMfcDoc diagnostics

#ifdef _DEBUG
void CBigJsonTreeViewMfcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBigJsonTreeViewMfcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBigJsonTreeViewMfcDoc commands
