#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include <QtCore/QString>

namespace CamelotEditor
{
	class ProjectPrefs : public CamelotEngine::Module<ProjectPrefs>
	{
	public:
		void save(const QString& path, bool overwrite = true) const;
		void load(const QString& path);

		const QString& getProjectName() const;
		void setProjectName(const QString& name);

	private:
		QString mName;

		void clear();
	};

	ProjectPrefs& gProjectPrefs();
}