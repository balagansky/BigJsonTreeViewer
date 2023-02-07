#pragma once

#include <memory>
#include <string>

struct OpenJsonFile;
class CViewTree;
class JsonTreeViewControllerImpl;

class JsonTreeViewController
{
public:
	JsonTreeViewController();

	void SetTreeCtrl(CViewTree &treeCtrl);

	void Update(const OpenJsonFile &file);
	void Clear();
	void ExpandItem(HTREEITEM item);

private:
	std::shared_ptr<JsonTreeViewControllerImpl> m_Impl;
};

extern JsonTreeViewController gJsonTreeViewController;
