#include "JsonTreeViewControllerImpl.h"
#include "Log.h"
#include "ViewTree.h"
#include "Bread/OpenFileManager.h"
#include "Bread/Timer.h"

// for now just assume we're dealing with rapidjson DOM...
// TODO: generalize to support multiple kinds of memory representations
#include "Bread/RapidJsonDomFileState.h"

#include <format>
#include <ranges>

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

void JsonTreeViewControllerImpl::SetTreeCtrlForFile(
	const std::filesystem::path& path, CViewTree& treeCtrl)
{
	File& treeFile = m_Files[path];
	treeFile.m_TreeCtrl = &treeCtrl;
	InitTree(treeFile);
}

void JsonTreeViewControllerImpl::SetFile(const OpenJsonFile& file)
{
	File& treeFile = m_Files[file.path];
	treeFile.m_File = &file;
	InitTree(treeFile);
}

void JsonTreeViewControllerImpl::InitTree(File& file)
{
	if (file.m_Initialized || !file.m_TreeCtrl || !file.m_File)
		return;

	file.m_Initialized = true;

	Log::Info("Updating tree view to new file..");

	Timer timer;

	{
		TreeLock treeLock(*file.m_TreeCtrl);

		file.m_TreeCtrl->DeleteAllItems();

		const RapidJsonDomFile& domFile = static_cast<const RapidJsonDomFile&>(*file.m_File->file);

		file.AddValue(domFile.m_Doc, 0);
	}

	Log::Info(std::format("Updated tree view in {:.2f}ms", timer.Ms()));
}

void JsonTreeViewControllerImpl::RemoveFile(const OpenJsonFile& file)
{
	m_Files.erase(file.path);
}

void JsonTreeViewControllerImpl::RemoveTreeCtrl(CViewTree& treeCtrl)
{
	std::erase_if(m_Files,
		[&](const auto& fileEntry) { return fileEntry.second.m_TreeCtrl == &treeCtrl; });
}

void JsonTreeViewControllerImpl::ExpandItem(CViewTree& treeCtrl, HTREEITEM item)
{
	auto files = m_Files | std::views::values;
	auto foundFile = std::ranges::find_if(files,
		[&](const File& file) { return file.m_TreeCtrl == &treeCtrl; });

	if (foundFile == files.end())
		return (void)Log::Error("Can't find tree whose item is being expanded!");

	File& file = *foundFile;

	if (file.m_ItemMap.empty())
		return; // not initialized yet

	ExpandableItem& expandableItem = file.m_ItemMap.at(item);
	if (!expandableItem.placeholder)
		return; // we already filled this previously

	Log::Info("Expanding an item");

	Timer timer;

	{
		TreeLock treeLock(treeCtrl);

		treeCtrl.DeleteItem(expandableItem.placeholder);
		expandableItem.placeholder = 0;

		file.AddValue(*expandableItem.value, item);
	}

	Log::Info(std::format("Updated tree view in {:.2f}ms", timer.Ms()));
}

void JsonTreeViewControllerImpl::File::AddValue(
	const rapidjson::Value& value, HTREEITEM where)
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
			*const_cast<char*>(value.GetString() + value.GetStringLength()) = 0;
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
				m_ItemMap.emplace(memberItem, ExpandableItem{ &member.value, placeholder });
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
				m_ItemMap.emplace(memberItem, ExpandableItem{ &arrValue, placeholder });
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
