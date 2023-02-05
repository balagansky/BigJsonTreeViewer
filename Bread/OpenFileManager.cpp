#include "OpenFileManager.h"
#include "FileLoader.h"

OpenFileManager gOpenFileManager;

void OpenFileManager::OpenFile(const std::filesystem::path& path)
{
	if (auto loadedFile = FileLoader::Load(path))
	{
		OpenJsonFile openFile = { .filename = path.filename().string(),
			.file{loadedFile}
		};
		m_OpenFiles.push_back(std::move(openFile));
	}
}
