#include "CmEditorPrefs.h"

namespace CamelotEditor
{
	UINT32 EditorPrefs::getNumRecentlyUsedProjects() const
	{
		return (UINT32)mRecentlyUsedProjects.size();
	}

	const QString& EditorPrefs::getRecentlyUsedProjectPath(UINT32 idx) const
	{
		return mRecentlyUsedProjects.at(idx);
	}

	void EditorPrefs::addRecentlyUsedProjectPath(const QString& path)
	{
		mRecentlyUsedProjects.push_back(path);
	}

	void EditorPrefs::removeRecentlyUsedProjectPath(UINT32 idx)
	{
		mRecentlyUsedProjects.erase(mRecentlyUsedProjects.begin() + idx);
	}

	void EditorPrefs::setLastUsedProjectDirectory(const QString& value)
	{
		mLastUsedProjectDirectory = value;
	}

	const QString& EditorPrefs::getLastUsedProjectDirectory() const
	{
		return mLastUsedProjectDirectory;
	}

	EditorPrefs& gEditorPrefs()
	{
		return EditorPrefs::instance();
	}
}