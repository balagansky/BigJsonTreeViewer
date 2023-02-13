//#pragma optimize("", off)
#include "Search.h"
#include "OpenFileManager.h"
#include "Butter/Log.h"
#include "RapidJsonDomFileState.h"
#include "SearchResult.h"
#include "Butter/SearchResults.h"

#include <cassert>
#include <string_view>

class SearchHandler {
	using JsonSize = rapidjson::SizeType;
public:
	SearchHandler(const std::string& searchString, 
		const std::filesystem::path &filePath) : m_SearchString(searchString)
	{
		m_Current.filePath = filePath;
	}

	bool StartObject() {
		m_Current.path.emplace_back(); return true;
	}
	bool EndObject(JsonSize) {
		m_Current.path.pop_back(); Increment(); return true;
	}
	bool Key(const char* key, JsonSize len, bool) {
		m_Current.path.back().SetKey(key); return CheckMatch({ key, len }, false);
	}
	bool StartArray() {
		InitPath(); m_Current.path.back().index = 0; return true;
	}
	bool EndArray(JsonSize) {
		Increment(); return true;
	}
	bool String(const char* str, JsonSize len, bool) {
		InitPath();
		bool shouldContinue = CheckMatch({ str,len }, true);
		Increment();
		return shouldContinue;
	}

#define JTV_IGNORE(func, type) bool func(type) { InitPath(); Increment(); return true; }
	JTV_IGNORE(Null, );
	JTV_IGNORE(Bool, bool);
	JTV_IGNORE(Double, double);
	JTV_IGNORE(Int, int);
	JTV_IGNORE(Uint, unsigned);
	JTV_IGNORE(Int64, int64_t);
	JTV_IGNORE(Uint64, uint64_t);

private:
	void InitPath() {
		// TODO: avoid the need for this by handling single-value file cases up front
		if (m_Current.path.empty())
			m_Current.path.emplace_back();
	}
	void Increment() {
		auto& optIndex = m_Current.path.back().index;
		if (optIndex)++* optIndex;
	}
	bool CheckMatch(std::string_view fileStr, bool isValue)
	{
		if (fileStr.find(m_SearchString) == std::string_view::npos)
			return true;

		if (isValue)
			m_Current.value = fileStr;

		std::ranges::for_each(m_Current.path, &SearchResult::Frame::Bake);
		return SearchResults::AddResult(m_Current);
	}

private:
	SearchResult m_Current;
	const std::string& m_SearchString;
};

void Search(const std::string& searchString)
{
	const OpenJsonFile *activeFile = gOpenFileManager.m_ActiveFile;

	if (!activeFile)
	{
		assert(false);
		Log::Error("Can't search - no active file.");
		return;
	}

	const RapidJsonDomFile& domFile = static_cast<decltype(domFile)>(*activeFile->file);

	SearchHandler searchHandler(searchString, activeFile->path);

	SearchResults::SearchStarted(searchString.c_str());
	domFile.m_Doc.Accept(searchHandler);
	SearchResults::SearchFinished();
}
