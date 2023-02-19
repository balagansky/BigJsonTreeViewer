#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

// represents a location in a JSON file
struct SearchResult {
	using JsonSize = unsigned int; // same as rapidjson::SizeType
	struct Frame {
		void SetKey(const char *key) { this->key = key; }
		void Bake() { keyStr = key; }

		const char* Key() const { return keyStr.empty() ? key : keyStr.c_str(); }
		std::vector<JsonSize> indices; // size > 1 in case of nested arrays
	private:
		const char* key = "";
		std::string keyStr;
	};
	std::vector<Frame> path;
	std::string value;

	std::filesystem::path filePath;
};
