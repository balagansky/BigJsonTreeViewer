#include "FileLoader.h"
#include "Butter/Log.h"

const JsonNode* FileLoader::Load(const std::filesystem::path& path)
{
	Log::Info(std::string() + "Loading " + path.string().c_str());

	return nullptr;
}
