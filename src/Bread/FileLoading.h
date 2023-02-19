#pragma once

#include <filesystem>

struct JsonFile;

namespace FileLoading
{
	static std::shared_ptr<const JsonFile> Load(const std::filesystem::path &path);
}