#pragma once

#include "rapidjson/document.h"

#include <unordered_map>

struct OpenJsonFile;
class CViewTree;

class JsonTreeViewControllerImpl
{
	friend class JsonTreeViewController;

	void Update(const OpenJsonFile &file);
	void Clear();
	void ExpandItem(HTREEITEM item);

private:
	void AddValue(const rapidjson::Value &value, HTREEITEM where);

private:

private:
	CViewTree *m_TreeCtrl = nullptr;

	struct ExpandableItem {
		const rapidjson::Value* value;
		HTREEITEM placeholder;
	};
	std::unordered_map<HTREEITEM, ExpandableItem> m_ItemMap;
};

