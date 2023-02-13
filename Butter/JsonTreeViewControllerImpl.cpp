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

namespace {
	using JsonSize = rapidjson::SizeType;
}

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

// TODO: make these into settings
constexpr JsonSize cNumInitiallyExpanded = 10;
constexpr JsonSize cMinItemsForFolding = 100;

JsonSize GetSubrangeSize(JsonSize numItems)
{
	if (numItems < 20)
		return numItems;
	JsonSize powerOfTenDivisor = (JsonSize)std::pow(10, std::ceil(std::log10(numItems)) - 1);
	if (numItems >= powerOfTenDivisor*2)
		return powerOfTenDivisor;
	else
		return powerOfTenDivisor / 10;
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

	Timer timer;

	{
		TreeLock treeLock(treeCtrl);

		if (auto range = expandableItem.value.arrayRange)
		{
			assert(range->start < range->end);
			JsonSize numItems = range->end - range->start;
			JsonSize subRangeSize = GetSubrangeSize(numItems);

			if (subRangeSize == numItems)
			{
				file.AddArrayValues(expandableItem.value.value->GetArray(),
					range->start, range->end, treeCtrl.GetParentItem(item), item);
			}
			else
			{
				HTREEITEM where = item;
				for (JsonSize start = range->start;;)
				{
					JsonSize end =
						std::min((start + subRangeSize)/subRangeSize*subRangeSize, range->end);
					if (range->end - end < subRangeSize)
						end = range->end;

					where = file.AddRange(treeCtrl.GetParentItem(where), start, end,
						*expandableItem.value.value, where);

					if (end == range->end)
						break;

					start = end;
				}
			}
			treeCtrl.DeleteItem(item);
			file.m_ItemMap.erase(item);
		}
		else
		{
			treeCtrl.DeleteItem(expandableItem.placeholder);
			expandableItem.placeholder = 0;

			file.AddValue(*expandableItem.value.value, item);
		}
	}

	//Log::Info(std::format("Updated tree view in {:.2f}ms", timer.Ms()));
}

static const char* JsonValueStr(const rapidjson::Value& value) {
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

void JsonTreeViewControllerImpl::File::AddValue(
	const rapidjson::Value& value, HTREEITEM parent)
{
	switch (value.GetType())
	{
	case rapidjson::kObjectType:
	{
		size_t i = 0;
#undef GetObject
		for (const auto& member : value.GetObject())
		{
			HTREEITEM memberItem = m_TreeCtrl->InsertItem(
				std::format("{} : {}", member.name.GetString(), JsonValueStr(member.value)).c_str(),
				0, 0, parent);
			if (member.value.IsObject() || member.value.IsArray())
			{
				MakeExpandable(memberItem, { &member.value });
			}
			// TODO: handle similarly to arrays
			if (++i > 10000)
			{
				m_TreeCtrl->InsertItem(
					std::format("{{{} more not shown}}", value.GetArray().Size() - i).c_str(),
					0, 0, parent);
				break; // can't handle too many atm
			}
		}
		break;
	}
	case rapidjson::kArrayType:
	{
		JsonSize numArrayItems = value.GetArray().Size();
		JsonSize numInitial = cNumInitiallyExpanded;
		if (numArrayItems < cMinItemsForFolding)
			numInitial = numArrayItems;

		AddArrayValues(value.GetArray(), 0, numInitial, parent);

		if (numInitial < numArrayItems) {
			AddRange(parent, numInitial, numArrayItems, value);
		}

		break;
	}

	default:
		m_TreeCtrl->InsertItem(JsonValueStr(value), 0, 0, parent);
	}
}

HTREEITEM JsonTreeViewControllerImpl::File::AddRange(HTREEITEM parent,
	rapidjson::SizeType start, rapidjson::SizeType end,
	const rapidjson::Value &value, HTREEITEM where)
{
	HTREEITEM rangeItem = m_TreeCtrl->InsertItem(
		std::format("[{} - {}] ...", start, end - 1).c_str(),
		0, 0, parent, where);
	MakeExpandable(rangeItem, { &value, {{start, end}} });
	return rangeItem;
}

void JsonTreeViewControllerImpl::File::AddArrayValues(rapidjson::Value::ConstArray array,
	rapidjson::SizeType start, rapidjson::SizeType end, HTREEITEM parent,
	HTREEITEM where)
{
	for (JsonSize i = start; i < end; ++i)
	{
		const rapidjson::Value& arrValue = array[i];
		HTREEITEM arrayItem = m_TreeCtrl->InsertItem(
			std::format("[{}] : {}", i, JsonValueStr(arrValue)).c_str(),
			0, 0, parent, where);
		if (arrValue.IsObject() || arrValue.IsArray())
		{
			MakeExpandable(arrayItem, { &arrValue });
		}
		where = arrayItem;
	}
}

void JsonTreeViewControllerImpl::File::MakeExpandable(HTREEITEM item,
	ExpandableItem::Value expansionValue)
{
	HTREEITEM placeholder = m_TreeCtrl->InsertItem("", 0, 0, item);
	m_ItemMap.emplace(item, ExpandableItem{ expansionValue, placeholder });
}
