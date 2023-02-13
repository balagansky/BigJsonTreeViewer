#include "SearchResults.h"
#include "JsonTreeViewController.h"
#include "Bread/SearchResult.h"

#include <format>

namespace {
	unsigned int gCurrentResultCount = 0;
	constexpr unsigned int cMaxResults = 100;

	SearchResults::SearchResultCallbacks gCallbacks;

	std::unordered_map<int, SearchResult> gSearchResults;
}

void SearchResults::SetCallbacks(SearchResultCallbacks callbacks)
{
	gCallbacks = std::move(callbacks);
}

void SearchResults::SearchStarted(const char* searchString)
{
	gCurrentResultCount = 0;
	gSearchResults.clear();
	gCallbacks.clear();
	gCallbacks.focus();
	gCallbacks.addLine(std::format("Searching for \"{}\"...", searchString).c_str());
}

void SearchResults::SearchFinished() {
	if (gCurrentResultCount < cMaxResults)
		gCallbacks.addLine("Search finished.");
}

bool SearchResults::AddResult(const SearchResult& searchResult)
{
	if (++gCurrentResultCount > cMaxResults)
	{
		gCallbacks.addLine("Max result display exceeded. Stopping search.");
		return false;
	}

	std::string str;
	for (const SearchResult::Frame& frame : searchResult.path)
	{
		if (!str.empty())
			str += " : ";

		if (*frame.Key())
			str += std::string() + '\"' + frame.Key() + '\"';

		if (frame.index)
			str += std::format("[{}]", *frame.index);
	}

	if (!searchResult.value.empty())
		str += std::format(" : {}", searchResult.value);

	gSearchResults[gCallbacks.addLine(str.c_str())] = searchResult;

	return true;
}

void SearchResults::SelectResultLine(int line)
{
	if (gSearchResults.contains(line))
		gJsonTreeViewController.SelectSearchResult(gSearchResults.at(line));
}
