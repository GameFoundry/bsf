#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include <QtCore/QString>
#include "3rdParty/pugixml/pugixml.hpp"

namespace CamelotEditor
{
	struct WindowLayoutDesc
	{
		WindowLayoutDesc()
			:width(0), height(0), left(0), top(0), 
			screenIdx(-1), maximized(true), docked(false)
		{

		}

		QString name;
		UINT32 width;
		UINT32 height;
		UINT32 left;
		UINT32 top;
		UINT32 screenIdx;
		bool maximized;
		bool docked;
	};

	class EditorPrefs : public CamelotEngine::Module<EditorPrefs>
	{
	public:
		UINT32 getNumRecentlyUsedProjects() const;
		const QString& getRecentlyUsedProjectPath(UINT32 idx) const;
		void addRecentlyUsedProjectPath(const QString& path);
		void removeRecentlyUsedProjectPath(UINT32 idx);

		void setLastUsedProjectDirectory(const QString& value);
		const QString& getLastUsedProjectDirectory() const;

		void setMainWindowLayout(const WindowLayoutDesc& desc);
		const WindowLayoutDesc& getMainWindowLayout() const;

		void save(const QString& path, bool overwrite = true) const;
		void load(const QString& path);

	private:
		vector<QString>::type mRecentlyUsedProjects;
		QString mLastUsedProjectDirectory;

		WindowLayoutDesc mMainWindowLayout;

		void saveWindowLayout(pugi::xml_node parentNode, const WindowLayoutDesc& desc) const;
		WindowLayoutDesc loadWindowLayout(pugi::xml_node node) const;
		void clear();
	};

	EditorPrefs& gEditorPrefs();
}