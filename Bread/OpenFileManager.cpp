#include "OpenFileManager.h"
#include "FileLoader.h"
#include "Butter/JsonTreeViewController.h"

#include <algorithm>
#include <ranges>

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

		gJsonTreeViewController.SetFile(*m_ActiveFile);

		return true;
	}

	return false;
}

void OpenFileManager::CloseFile(const std::filesystem::path& path)
{
	auto foundOpenFile = std::ranges::find_if(m_OpenFiles,
		[&](const OpenJsonFile &file) { return path == file.path; });

	if (foundOpenFile == m_OpenFiles.end())
		return;

	gJsonTreeViewController.RemoveFile(*foundOpenFile);

	m_OpenFiles.erase(foundOpenFile);

	m_ActiveFile = nullptr;
}

void OpenFileManager::SetActiveFile(const std::filesystem::path& path)
{
	if (m_ActiveFile && m_ActiveFile->path == path)
		return;

	for (const OpenJsonFile &file : m_OpenFiles) {
		if (file.path == path)
			m_ActiveFile = &file;
	}

	// no action for now
	/*if (m_ActiveFile)
		gJsonTreeViewController.Update(*m_ActiveFile);*/
}