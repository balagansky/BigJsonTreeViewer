#include "FileLoader.h"
#include "Butter/Log.h"
#include "FileState.h"

// windows defined
#undef min
#undef max

#define RAPIDJSON_SIMD
#define RAPIDJSON_SSE42

#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>
#include <rapidjson/istreamwrapper.h>

#include <fstream>
#include <format>
#include <chrono>

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

struct Timer {
	Timer() {
		m_Start = std::chrono::high_resolution_clock::now();
	}

	double Ms(bool reset = true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		double ms = std::chrono::duration<double>(
			std::chrono::high_resolution_clock::now() - m_Start).count() * 1000;
		if (reset)
			m_Start = now;
		return ms;
	}

	decltype(std::chrono::high_resolution_clock::now()) m_Start;
};


std::shared_ptr<const JsonFile> FileLoader::Load()
{
	std::ifstream fileStream(m_Path);
	if (!fileStream.is_open() || !fileStream.good())
		return Log::Error(std::string() + "Failed to open " + m_Path.string().c_str());

	Log::Info(std::string() + "Loading " + m_Path.string().c_str());

	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	std::string fileStr = buffer.str();

	auto startTime = std::chrono::high_resolution_clock::now();

	auto parsedFile = std::make_shared<JsonFile>();

	Timer timer;

	//rapidjson::IStreamWrapper saxStream(fileStream);
	rapidjson::InsituStringStream saxStream(const_cast<char*>(fileStr.c_str()));
	SaxHandler saxHandler(*parsedFile);
	rapidjson::Reader saxReader;
	rapidjson::ParseResult result = saxReader.Parse
		<rapidjson::kParseNumbersAsStringsFlag | rapidjson::kParseInsituFlag>(
		saxStream, saxHandler);

	if (!result)
		return Log::Error(std::format("Parse failure at {}: {}", result.Offset(),
			GetParseError_En(result.Code())));

	Log::Info(std::format("Loaded in {}ms", timer.Ms()));

	return parsedFile;
}
