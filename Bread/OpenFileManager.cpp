#include "OpenFileManager.h"
#include "FileLoader.h"

OpenFileManager gOpenFileManager;

bool OpenFileManager::OpenFile(const std::filesystem::path& path)
{
	if (auto loadedFile = FileLoader::Load(path))
	{
		OpenJsonFile openFile = { .filename = path.filename().string(),
			.file{loadedFile}
		};
		m_OpenFiles.push_back(std::move(openFile));
		m_ActiveFile = &m_OpenFiles.back();

		return true;
	}

	return false;
}
