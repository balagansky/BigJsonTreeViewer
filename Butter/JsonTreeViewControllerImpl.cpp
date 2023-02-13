#include "JsonTreeViewControllerImpl.h"
#include "Log.h"
#include "ViewTree.h"
#include "Bread/OpenFileManager.h"
#include "Bread/Timer.h"
#include "Bread/SearchResult.h"
#include "Bread/Utils.h"

// for now just assume we're dealing with rapidjson DOM...
// TODO: generalize to support multiple kinds of memory representations
#include "Bread/RapidJsonDomFileState.h"

#include <format>
#include <functional>
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
	if (numItems >= powerOfTenDivisor * 2)
		return powerOfTenDivisor;
	else
		return powerOfTenDivisor / 10;
}

void JsonTreeViewControllerImpl::ExpandItem(CViewTree& treeCtrl, HTREEITEM item,
	std::optional<rapidjson::SizeType> index)
{
	auto files = m_Files | std::views::values;
	auto foundFile = std::ranges::find_if(files,
		[&](const File& file) { return file.m_TreeCtrl == &treeCtrl; });

	if (foundFile == files.end())
		return (void)Log::Error("Can't find tree whose item is being expanded!");

	File& file = *foundFile;

	if (file.m_ExpandableItems.empty())
		return; // not initialized yet

	ExpandableItem& expandableItem = file.m_ExpandableItems.at(item);
	if (!expandableItem.placeholder)
		return; // we already filled this previously

	Timer timer;

	{
		TreeLock treeLock(treeCtrl);

		if (auto range = expandableItem.value.arrayRange)
		{
			if (index) {
				// if a specific index is requested, split the placeholder into 1-2 new ranges
				// with the specified index fully expanded in between
				// do this in reverse so we can use the same insert location

				auto fAddSubRange = [&](JsonSize start, JsonSize end) {
					JsonSize rangeSize = end - start;
					if (rangeSize == 1)
					{
						file.AddArrayValues(expandableItem.value.value->GetArray(),
							start, end, treeCtrl.GetParentItem(item), item);
					}
					else if (rangeSize > 1)
					{
						file.AddRange(treeCtrl.GetParentItem(item), start, end,
							*expandableItem.value.value, item);
					}
				};

				fAddSubRange(*index + 1, range->end);
				file.AddArrayValues(expandableItem.value.value->GetArray(),
					*index, *index + 1, treeCtrl.GetParentItem(item), item);
				fAddSubRange(range->start, *index);
			}
			else {
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
							std::min((start + subRangeSize) / subRangeSize * subRangeSize, range->end);
						if (range->end - end < subRangeSize)
							end = range->end;

						where = file.AddRange(treeCtrl.GetParentItem(where), start, end,
							*expandableItem.value.value, where);

						if (end == range->end)
							break;

						start = end;
					}
				}
			}
			std::erase(file.m_JsonValueToTreeItems[expandableItem.value.value], item);
			treeCtrl.DeleteItem(item);
			file.m_ExpandableItems.erase(item);
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

void JsonTreeViewControllerImpl::SelectSearchResult(const SearchResult& searchResult)
{
	if (!m_Files.contains(searchResult.filePath))
	{
		// TODO: hook file opening here?
		return;
	}

	File& file = m_Files.at(searchResult.filePath);

	file.m_TreeCtrl->SetFocus();

	auto& doc = static_cast<const RapidJsonDomFile&>(*file.m_File->file).m_Doc;
	const rapidjson::Value* curVal = &doc;
	for (size_t iFrame = 0; iFrame < searchResult.path.size(); /*incremented at the end*/) {
		const SearchResult::Frame& frame = searchResult.path[iFrame];

		if (curVal != &doc && !Log::Invariant(file.m_JsonValueToTreeItems.contains(curVal),
			"Couldn't locate tree item while attempting to display search result."))
			return;

		auto fCheckFrameMatchAndExpand = [&](HTREEITEM treeItem, bool ignorePlaceholders = false)
		{
			if (std::optional<ExpandableItem> expItem = Utils::Lookup(
				file.m_ExpandableItems, treeItem))
			{
				if (expItem->value.arrayRange)
				{
					// this is a placeholder array range.. check if our search result is in it
					if (Utils::InRange(frame.index, expItem->value.arrayRange->start,
						expItem->value.arrayRange->end - 1))
					{
						// search result is in the placeholder range, so let's expand it
						ExpandItem(*file.m_TreeCtrl, treeItem, frame.index);
						return true;
					}
				}
				else if (!ignorePlaceholders) {
					// this is a placeholder object... expand it
					ExpandItem(*file.m_TreeCtrl, treeItem);
					return true;
				}
			}
			else if (std::optional<rapidjson::SizeType> index = Utils::Lookup(
				file.m_ArrayMemberItems, treeItem))
			{
				// search result corresponds to an already expanded array index
				return true;
			}
			else
			{
				// must be a leaf and must be what we're looking for
				return true;
			}
			return false; // note: bad indentation is due to VS formatting bug
		};

		const auto& valItems = file.m_JsonValueToTreeItems[curVal];
		std::optional<HTREEITEM> foundItem = Utils::FindIf(
			valItems, fCheckFrameMatchAndExpand);

		// TRICKY: for object member arrays, we need to repeat the search again
		//	The first time through will have expanded the object member, but not
		//	the frame's array index.
		Utils::FindIf(
			valItems, std::bind(fCheckFrameMatchAndExpand, std::placeholders::_1, true));

		if (foundItem)
			file.m_TreeCtrl->SelectItem(*foundItem);

		switch (curVal->GetType())
		{
		case rapidjson::kObjectType:
			curVal = &curVal->FindMember(frame.Key())->value;
			assert(curVal);
			// TRICKY: every iteration of this loop deals with either a leaf, an object member, or
			//  an array element. But some frames can have both a key (object member) and an array
			//  index. In this special case, we need to repeat the loop with the same frame.
			if (!curVal->IsArray())
			{
				++iFrame;
			}
			else
			{
				assert(frame.index.has_value());
			}
			break;
		case rapidjson::kArrayType:
			curVal = &curVal->GetArray()[*frame.index];
			++iFrame;
			break;
		default:
			Log::Invariant(false, "Encountered unexpected value type when displaying search result.");
			return;
		}
	}

	std::optional<std::vector<HTREEITEM>> resultItems = Utils::Lookup(
		file.m_JsonValueToTreeItems, curVal);
	if (!Log::Invariant(resultItems.has_value(), "Can't find leaf result item."))
		return;

	// select final value if it a leaf (might not be)
	if (resultItems->size() == 1)
		file.m_TreeCtrl->SelectItem(resultItems->front());
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
			m_JsonValueToTreeItems[&member.value].push_back(memberItem);
			if (member.value.IsObject() || member.value.IsArray())
			{
				MakeExpandable(memberItem, { &member.value });
			}
			// TODO: handle similarly to arrays
			if (++i > 10000)
			{
				m_JsonValueToTreeItems[&member.value].push_back(
					m_TreeCtrl->InsertItem(
						std::format("{{{} more not shown}}", value.GetArray().Size() - i).c_str(),
						0, 0, parent));
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
		m_JsonValueToTreeItems[&value].push_back(
			m_TreeCtrl->InsertItem(JsonValueStr(value), 0, 0, parent));
	}
}

HTREEITEM JsonTreeViewControllerImpl::File::AddRange(HTREEITEM parent,
	rapidjson::SizeType start, rapidjson::SizeType end,
	const rapidjson::Value& value, HTREEITEM where)
{
	HTREEITEM rangeItem = m_TreeCtrl->InsertItem(
		std::format("[{} - {}] ...", start, end - 1).c_str(),
		0, 0, parent, where);
	MakeExpandable(rangeItem, { &value, {{start, end}} });
	m_JsonValueToTreeItems[&value].push_back(rangeItem);
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
		m_JsonValueToTreeItems[&arrValue].push_back(arrayItem);
		m_ArrayMemberItems.emplace(arrayItem, i);
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
	m_ExpandableItems.emplace(item, ExpandableItem{ expansionValue, placeholder });
}
