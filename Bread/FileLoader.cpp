#include "FileLoader.h"
#include "Butter/Log.h"
#include "FileState.h"
#include "Timer.h"

// windows defined
#undef min
#undef max

#define RAPIDJSON_SIMD
#define RAPIDJSON_SSE42

#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <fstream>
#include <format>

class SaxHandler : public rapidjson::BaseReaderHandler < rapidjson::UTF8<>, SaxHandler >
{
public:
	SaxHandler(JsonFile& outFile) : m_OutFile(outFile) {}

	bool Default() {
		assert(false); return false;
	}

	bool Key(const char* str, size_t len, bool) {
		AddString(str, len);
		return true;
	}

	bool StartObject() {
		return true;
	}

	bool EndObject(size_t /*memberCount*/) {
		return true;
	}

	bool StartArray() {
		return true;
	}

	bool EndArray(size_t /*memberCount*/) {
		return true;
	}

	bool String(const char* str, size_t len, bool) {
		AddString(str, len);
		return true;
	}

	bool RawNumber(const char* str, size_t len, bool copy) {
		return String(str, len, copy);
	}

	bool Bool(bool b) {
		if (b)
			AddString("true", 4u);
		else
			AddString("false", 5u);
		return true;
	}

	bool Null() {
		return true;
	}

	void AddString(const char* str, size_t len)
	{
		/*std::string& mapStr = m_OutFile.strings[str];
		if (mapStr.empty())
			mapStr = std::string(str, len);*/
	}

private:
	JsonFile& m_OutFile;

	// state
	JsonNode* m_Node;
};

std::vector<char> ReadFileIntoBuffer(const std::filesystem::path &path) {
    std::vector<char> buffer;

    HANDLE hFile = CreateFile(path.string().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // handle error
        return buffer;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    buffer.resize(dwFileSize+1);
	buffer.back() = 0;

    DWORD dwBytesRead;
    if (!ReadFile(hFile, buffer.data(), dwFileSize, &dwBytesRead, NULL)) {
        // handle error
        buffer.clear();
    }

    CloseHandle(hFile);
    return buffer;
}

std::shared_ptr<const JsonFile> FileLoader::Load()
{
	Log::Info(std::string() + "Loading " + m_Path.string().c_str());

	Timer timer;

	std::vector<char> buffer = ReadFileIntoBuffer(m_Path);

	if (buffer.empty())
		return Log::Error(std::string() + "Failed to open " + m_Path.string().c_str());

	Log::Info(std::format("Loaded in {:.2f}ms", timer.Ms()));

	auto parsedFile = std::make_shared<JsonFile>();

	rapidjson::ParseResult result;

	if (true)
	{
		// insitu dom
		rapidjson::Document doc;
		result = doc.Parse<rapidjson::kParseNumbersAsStringsFlag | rapidjson::kParseInsituFlag>(const_cast<char*>(
			buffer.data()));
	}
	else if (true)
	{
		//rapidjson::IStreamWrapper saxStream(fileStream);
		rapidjson::InsituStringStream saxStream(const_cast<char*>(buffer.data()));
		SaxHandler saxHandler(*parsedFile);
		rapidjson::Reader saxReader;
		result = saxReader.Parse
			<rapidjson::kParseNumbersAsStringsFlag | rapidjson::kParseInsituFlag>(
			saxStream, saxHandler);
	}
	else
	{
		// insitu stream
		//rapidjson::IStreamWrapper saxStream(fileStream);
		rapidjson::InsituStringStream saxStream(const_cast<char*>(buffer.data()));
		SaxHandler saxHandler(*parsedFile);
		rapidjson::Reader saxReader;
		result = saxReader.Parse
			<rapidjson::kParseNumbersAsStringsFlag | rapidjson::kParseInsituFlag>(
			saxStream, saxHandler);
	}

	if (!result)
		return Log::Error(std::format("Parse failure at {}: {}", result.Offset(),
			GetParseError_En(result.Code())));

	Log::Info(std::format("Parsed in {:.2f}ms", timer.Ms()));

	return parsedFile;
}
