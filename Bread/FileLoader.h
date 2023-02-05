#pragma once

#include <filesystem>

struct JsonNode;

namespace FileLoader
{
	const JsonNode* Load(const std::filesystem::path &path);
}