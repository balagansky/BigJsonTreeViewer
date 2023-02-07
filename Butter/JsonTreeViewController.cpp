#include "JsonTreeViewController.h"
#include "JsonTreeViewControllerImpl.h"

JsonTreeViewController gJsonTreeViewController;

JsonTreeViewController::JsonTreeViewController()
{
	m_Impl = std::make_shared<JsonTreeViewControllerImpl>();
}

void JsonTreeViewController::SetTreeCtrlForFile(
	const std::filesystem::path &path, CViewTree &treeCtrl)
{
	m_Impl->SetTreeCtrlForFile(path, treeCtrl);
}

void JsonTreeViewController::SetFile(const OpenJsonFile &file)
{
	m_Impl->SetFile(file);
}

void JsonTreeViewController::RemoveFile(const OpenJsonFile &file)
{
	m_Impl->RemoveFile(file);
}

void JsonTreeViewController::RemoveTreeCtrl(CViewTree &treeCtrl)
{
	m_Impl->RemoveTreeCtrl(treeCtrl);
}

void JsonTreeViewController::ExpandItem(CViewTree &treeCtrl, HTREEITEM item)
{
	m_Impl->ExpandItem(treeCtrl, item);
}
