#pragma once

#include "FileState.h"

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

struct OpenJsonFile {
	std::string filename;
	std::shared_ptr<const JsonNode> root;
};

struct OpenFileManager
{
	void OpenFile(const std::filesystem::path &path);

	std::vector<OpenJsonFile> m_OpenFiles;
};

extern OpenFileManager gOpenFileManager;
