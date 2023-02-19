#include "FileLoading.h"
#include "FileLoader.h"

static std::shared_ptr<const JsonFile> FileLoading::Load(const std::filesystem::path& path)
{
	return FileLoader::Load(path);
}
