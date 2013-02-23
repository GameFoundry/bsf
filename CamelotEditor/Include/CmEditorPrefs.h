#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class EditorPrefs : public CamelotEngine::Module<EditorPrefs>
	{
	public:
		UINT32 getNumRecentlyUsedProjects() const;
		const QString& getRecentlyUsedProjectPath(UINT32 idx) const;
		void addRecentlyUsedProjectPath(const QString& path);
		void removeRecentlyUsedProjectPath(UINT32 idx);

		void setLastUsedProjectDirectory(const QString& value);
		const QString& getLastUsedProjectDirectory() const;

	private:
		vector<QString>::type mRecentlyUsedProjects;
		QString mLastUsedProjectDirectory;
	};

	EditorPrefs& gEditorPrefs();
}