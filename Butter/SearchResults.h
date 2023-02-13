#pragma once

#include <functional>

struct SearchResult;

namespace SearchResults {

	struct SearchResultCallbacks {
		std::function<void()> clear;
		std::function<void()> focus;
		std::function<int(const char*)> addLine;
	};
	void SetCallbacks(SearchResultCallbacks callbacks);

	void SearchStarted(const char* searchString);
	// returns false if no more results can be accepted
	bool AddResult(const SearchResult& searchResult);
	void SearchFinished();

	void SelectResultLine(int line);

}
