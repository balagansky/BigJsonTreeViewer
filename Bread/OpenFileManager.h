#pragma once

#include <filesystem>
#include <string>
#include <list>
#include <memory>

struct JsonFile;

struct OpenJsonFile {
	std::string filename;
	std::filesystem::path path;
	std::shared_ptr<const JsonFile> file;
};

struct OpenFileManager
{
	bool OpenFile(const std::filesystem::path &path);
	void CloseFile(const std::filesystem::path &path);

	void SetActiveFile(const std::filesystem::path &path);

	const OpenJsonFile *m_ActiveFile = nullptr;
	std::list<OpenJsonFile> m_OpenFiles;
};

extern OpenFileManager gOpenFileManager;
