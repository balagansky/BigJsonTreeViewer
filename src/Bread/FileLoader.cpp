#include "FileLoader.h"
#include "Butter/Log.h"
#include "RapidJsonDomFileState.h"
#include "Timer.h"

#include <rapidjson/error/en.h>
#include <rapidjson/document.h>

#include <fstream>
#include <format>

// returns (unaligned 1-byte) null terminated buffer containing file contents on success
//	(there could also be nulls in the middle of the buffer)
// returns empty buffer on failure
std::vector<char> ReadFileIntoBuffer(const std::filesystem::path& path)
{
	// open the file
	HANDLE hFile = CreateFile(path.string().c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return {};
	}

	// preallocate the buffer
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	std::vector<char> buffer;
	buffer.resize(dwFileSize + 1);

	// read file into buffer
	DWORD dwBytesRead;
	if (!ReadFile(hFile, buffer.data(), dwFileSize, &dwBytesRead, NULL)) {
		buffer.clear();
	}
	else
	{
		if (dwBytesRead != dwFileSize)
			buffer.resize(dwBytesRead+1);
		buffer.back() = 0;
	}

	// close file
	CloseHandle(hFile);
	return buffer;
}

std::shared_ptr<const JsonFile> FileLoader::Load()
{
	Log::Info(std::string() + "Loading " + m_Path.string().c_str());

	Timer timer;

	std::shared_ptr outFile = std::make_shared<RapidJsonDomFile>();

	outFile->m_Buffer = ReadFileIntoBuffer(m_Path);

	if (outFile->m_Buffer.empty())
		return Log::Error(std::string() + "Failed to open " + m_Path.string().c_str());

	Log::Info(std::format("Loaded in {:.2f}ms", timer.Ms()));

	rapidjson::ParseResult result;

	// insitu dom
	rapidjson::Document doc;
	result = outFile->m_Doc.ParseInsitu<rapidjson::kParseNumbersAsStringsFlag>(
		outFile->m_Buffer.data());


	if (!result)
		return Log::Error(std::format("Parse failure at {}: {}", result.Offset(),
			GetParseError_En(result.Code())));

	Log::Info(std::format("Parsed in {:.2f}ms", timer.Ms()));

	return outFile;
}
