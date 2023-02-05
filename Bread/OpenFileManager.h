#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

struct JsonFile;

struct OpenJsonFile {
	std::string filename;
	std::shared_ptr<const JsonFile> file;
};

struct OpenFileManager
{
	void OpenFile(const std::filesystem::path &path);

	std::vector<OpenJsonFile> m_OpenFiles;
};

extern OpenFileManager gOpenFileManager;
