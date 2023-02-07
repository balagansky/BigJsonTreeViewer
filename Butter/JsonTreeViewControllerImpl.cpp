#include "JsonTreeViewControllerImpl.h"
#include "Log.h"
#include "ViewTree.h"
#include "Bread/OpenFileManager.h"
#include "Bread/Timer.h"

// for now just assume we're dealing with rapidjson DOM...
// TODO: generalize to support multiple kinds of memory representations
#include "Bread/RapidJsonDomFileState.h"

#include <format>

struct TreeLock
{
	TreeLock(CViewTree& tree) : m_Tree(tree) {
		//m_TreeCtrl->LockWindowUpdate();
		m_Tree.SetRedraw(FALSE);
	}
	~TreeLock() {
		m_Tree.Invalidate();
		m_Tree.SetRedraw(TRUE);
		//m_TreeCtrl->UnlockWindowUpdate();
	}

	CViewTree& m_Tree;
};

void JsonTreeViewControllerImpl::Update(const OpenJsonFile& file)
{
	Log::Info("Updating tree view to new file..");

	Timer timer;

	if (!m_TreeCtrl)
		return (void)Log::Error("Oops! Don't have the tree control!");

	{
		TreeLock treeLock(*m_TreeCtrl);

		m_TreeCtrl->DeleteAllItems();

		const RapidJsonDomFile& domFile = static_cast<const RapidJsonDomFile&>(*file.file);

		AddValue(domFile.m_Doc, 0);
	}

	Log::Info(std::format("Updated tree view in {:.2f}ms", timer.Ms()));
}

void JsonTreeViewControllerImpl::Clear()
{
	m_TreeCtrl->DeleteAllItems();
}

void JsonTreeViewControllerImpl::ExpandItem(HTREEITEM item)
{
	if (m_ItemMap.empty())
		return; // not initialized yet

	ExpandableItem &expandableItem = m_ItemMap.at(item);
	if (!expandableItem.placeholder)
		return; // we already filled this previously

	Log::Info("Expanding an item");

	Timer timer;

	if (!m_TreeCtrl)
		return (void)Log::Error("Oops! Don't have the tree control!");

	{
		TreeLock treeLock(*m_TreeCtrl);

		m_TreeCtrl->DeleteItem(expandableItem.placeholder);
		expandableItem.placeholder = 0;

		AddValue(*expandableItem.value, item);
	}

	Log::Info(std::format("Updated tree view in {:.2f}ms", timer.Ms()));
}

void JsonTreeViewControllerImpl::AddValue(const rapidjson::Value& value, HTREEITEM where)
{
	auto fValueStr = [](const rapidjson::Value& value) {
		switch (value.GetType())
		{
		case rapidjson::kNullType:
			return "null";
		case rapidjson::kFalseType:
			return "false";
		case rapidjson::kTrueType:
			return "true";
		case rapidjson::kStringType:
			// HACK: when we use insitu + parseNumbersAsStrings, rapidjson does not
			//	add a null terminator after number strings because this will break its
			//	parsing. But we can do so safely here since we know the length.
			*const_cast<char*>(value.GetString()+value.GetStringLength()) = 0;
			return value.GetString();
		case rapidjson::kNumberType:
			return "unexpected number";
		case rapidjson::kObjectType:
			return "{...}";
		case rapidjson::kArrayType:
			return "[...]";
		}
		assert(false);
		return "oops";
	};

	switch (value.GetType())
	{
	case rapidjson::kObjectType:
	{
		size_t i = 0;
		#undef GetObject
		for (const auto& member : value.GetObject())
		{
			HTREEITEM memberItem = m_TreeCtrl->InsertItem(
				std::format("{} : {}", member.name.GetString(), fValueStr(member.value)).c_str(),
				0, 0, where);
			if (member.value.IsObject() || member.value.IsArray())
			{
				HTREEITEM placeholder = m_TreeCtrl->InsertItem("", 0, 0, memberItem);
				m_ItemMap.emplace(memberItem, ExpandableItem{&member.value, placeholder});
			}
			if (++i > 1000)
			{
				m_TreeCtrl->InsertItem(
					std::format("{{{} more not shown}}", value.GetArray().Size() - i).c_str(),
					0, 0, where);
				break; // can't handle too many atm
			}
		}
		break;
	}
	case rapidjson::kArrayType:
	{
		size_t i = 0;
		for (const rapidjson::Value& arrValue : value.GetArray())
		{
			HTREEITEM memberItem = m_TreeCtrl->InsertItem(
				std::format("[{}] : {}", i++, fValueStr(arrValue)).c_str(),
				0, 0, where);
			if (arrValue.IsObject() || arrValue.IsArray())
			{
				HTREEITEM placeholder = m_TreeCtrl->InsertItem("", 0, 0, memberItem);
				m_ItemMap.emplace(memberItem, ExpandableItem{&arrValue, placeholder});
			}

			if (i > 1000)
			{
				m_TreeCtrl->InsertItem(
					std::format("{{{} more not shown}}", value.GetArray().Size() - i).c_str(),
					0, 0, where);
				break; // can't handle too many atm
			}
		}
		break;
	}
	default:
		m_TreeCtrl->InsertItem(fValueStr(value), 0, 0, where);
	}
}
