#include "OpenFileManager.h"
#include "FileLoader.h"
#include "Butter/JsonTreeViewController.h"

#include <algorithm>

OpenFileManager gOpenFileManager;

bool OpenFileManager::OpenFile(const std::filesystem::path& path)
{
	if (auto loadedFile = FileLoader::Load(path))
	{
		OpenJsonFile openFile = {
			.filename = path.filename().string(),
			.path = path,
			.file{loadedFile}
		};
		m_OpenFiles.push_back(std::move(openFile));
		m_ActiveFile = &m_OpenFiles.back();

		gJsonTreeViewController.Update(*m_ActiveFile);

		return true;
	}

	return false;
}

void OpenFileManager::CloseFile(const std::filesystem::path& path)
{
	std::erase_if(m_OpenFiles, [&](const OpenJsonFile &file) { return path == file.path; });

	m_ActiveFile = nullptr;

	gJsonTreeViewController.Clear();
}

void OpenFileManager::SetActiveFile(const std::filesystem::path& path)
{
	if (m_ActiveFile && m_ActiveFile->path == path)
		return;

	for (const OpenJsonFile &file : m_OpenFiles) {
		if (file.path == path)
			m_ActiveFile = &file;
	}

	if (m_ActiveFile)
		gJsonTreeViewController.Update(*m_ActiveFile);
}