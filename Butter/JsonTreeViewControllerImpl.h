#pragma once

#include "rapidjson/document.h"

#include <filesystem>
#include <optional>
#include <unordered_map>

struct OpenJsonFile;
class CViewTree;

class JsonTreeViewControllerImpl
{
	friend class JsonTreeViewController;

	void SetTreeCtrlForFile(const std::filesystem::path& path, CViewTree& treeCtrl);
	void SetFile(const OpenJsonFile& file);

	void RemoveFile(const OpenJsonFile& file);
	void RemoveTreeCtrl(CViewTree& treeCtrl);

	void ExpandItem(CViewTree& treeCtrl, HTREEITEM item);

private:
	struct File;
	void InitTree(File& file);

private:

private:
	struct ExpandableItem {
		struct Value {
			struct Range {
				rapidjson::SizeType start;
				rapidjson::SizeType end; // exclusive
			};
			const rapidjson::Value* value;
			std::optional<Range> arrayRange;
		};

		Value value;
		HTREEITEM placeholder;
	};

	struct File {
		void AddValue(const rapidjson::Value& value, HTREEITEM parent);
		HTREEITEM AddRange(HTREEITEM parent, rapidjson::SizeType start, rapidjson::SizeType end,
			const rapidjson::Value &value, HTREEITEM where = TVI_LAST);
		void AddArrayValues(rapidjson::Value::ConstArray array, rapidjson::SizeType start,
			rapidjson::SizeType end, HTREEITEM parent, HTREEITEM where = TVI_LAST);
		void MakeExpandable(HTREEITEM item, ExpandableItem::Value expansionValue);

		bool m_Initialized = false;
		CViewTree* m_TreeCtrl = nullptr;
		const OpenJsonFile* m_File = nullptr;
		std::unordered_map<HTREEITEM, ExpandableItem> m_ItemMap;
	};
	std::unordered_map<std::filesystem::path, File> m_Files;
};

