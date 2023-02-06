#include "JsonTreeViewController.h"
#include "JsonTreeViewControllerImpl.h"

JsonTreeViewController gJsonTreeViewController;

JsonTreeViewController::JsonTreeViewController()
{
	m_Impl = std::make_shared<JsonTreeViewControllerImpl>();
}

void JsonTreeViewController::SetTreeCtrl(CViewTree &treeCtrl)
{
	m_Impl->m_TreeCtrl = &treeCtrl;
}

void JsonTreeViewController::Update(const OpenJsonFile &file)
{
	m_Impl->Update(file);
}

void JsonTreeViewController::ExpandItem(HTREEITEM item)
{
	m_Impl->ExpandItem(item);
}
