#pragma once

#include <filesystem>
#include <memory>
#include <string>

struct OpenJsonFile;
class CViewTree;
class JsonTreeViewControllerImpl;
struct SearchResult;

class JsonTreeViewController
{
public:
	JsonTreeViewController();

	void SetTreeCtrlForFile(const std::filesystem::path &path, CViewTree &treeCtrl);
	void SetFile(const OpenJsonFile &file);

	void RemoveFile(const OpenJsonFile &file);
	void RemoveTreeCtrl(CViewTree &treeCtrl);

	void ExpandItem(CViewTree &treeCtrl, HTREEITEM item);

	void SelectSearchResult(const SearchResult &searchResult);

private:
	std::shared_ptr<JsonTreeViewControllerImpl> m_Impl;
};

extern JsonTreeViewController gJsonTreeViewController;
