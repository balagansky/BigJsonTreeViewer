#include "SearchPrompt.h"

#include "FindDialog.h"
#include "Bread/Search.h"
#include "Bread/OpenFileManager.h"

void PromptSearch()
{
	if (!gOpenFileManager.m_ActiveFile)
		return; // TODO: prevent this from being called in the first place in this case

	FindDialog findDlg;
	findDlg.DoModal();

	if (findDlg.m_Confirmed)
		Search(findDlg.m_SearchString.GetString());
}
