#pragma once

#include <filesystem>
#include <memory>

struct JsonFile;

class FileLoader
{
public:
	static std::shared_ptr<const JsonFile> Load(const std::filesystem::path& path) {
		return FileLoader(path).Load();
	}

private:
	FileLoader(const std::filesystem::path& path) : m_Path(path) {}

	std::shared_ptr<const JsonFile> Load();

private:
	// input
	const std::filesystem::path& m_Path;
};