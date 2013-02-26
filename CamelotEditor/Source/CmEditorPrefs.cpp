#include "CmEditorPrefs.h"
#include "CmFileSystem.h"
#include <QtCore/QDir>
#include "3rdParty/pugixml/pugixml.hpp"

using namespace pugi;

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
		QString cleanPath = QDir::cleanPath(QDir::toNativeSeparators(path));

		for(auto iter = mRecentlyUsedProjects.begin(); iter != mRecentlyUsedProjects.end(); ++iter)
		{
			if(*iter == cleanPath)
				return;
		}

		mRecentlyUsedProjects.push_back(cleanPath);
	}

	void EditorPrefs::removeRecentlyUsedProjectPath(UINT32 idx)
	{
		mRecentlyUsedProjects.erase(mRecentlyUsedProjects.begin() + idx);
	}

	void EditorPrefs::setLastUsedProjectDirectory(const QString& value)
	{
		mLastUsedProjectDirectory = QDir::cleanPath(QDir::toNativeSeparators(value));
	}

	const QString& EditorPrefs::getLastUsedProjectDirectory() const
	{
		return mLastUsedProjectDirectory;
	}

	void EditorPrefs::save(const QString& path, bool overwrite) const
	{
		String stdPath = path.toStdString();

		if(FileSystem::fileExists(stdPath))
		{
			if(!overwrite)
			{
				CM_EXCEPT(FileNotFoundException, "File already exists at this location.");
			}
			else
				FileSystem::remove(stdPath);
		}

		xml_document xmldoc;
		xml_node camelotEditor = xmldoc.append_child("CamelotEditor");
		xml_node recentlyUsedProjects = camelotEditor.append_child("RecentlyUsedProjects");

		for(auto iter = mRecentlyUsedProjects.begin(); iter != mRecentlyUsedProjects.end(); ++iter)
		{
			xml_node recentlyUsedProject = recentlyUsedProjects.append_child("RecentlyUsedProject");
			xml_attribute pathAttrib = recentlyUsedProject.append_attribute("path");
			pathAttrib.set_value(iter->toStdString().c_str());
		}

		xml_node lastUsedProjectDirectory = camelotEditor.append_child("LastUsedProjectDir");
		xml_attribute pathAttrib = lastUsedProjectDirectory.append_attribute("path");
		pathAttrib.set_value(mLastUsedProjectDirectory.toStdString().c_str());

		xmldoc.save_file(stdPath.c_str());
	}

	void EditorPrefs::load(const QString& path)
	{
		clear();

		String stdPath = path.toStdString();

		if(!FileSystem::fileExists(stdPath))
			CM_EXCEPT(FileNotFoundException, "Specified file: " + stdPath + " does not exist.");

		xml_document xmldoc;
		xml_parse_result parseResult = xmldoc.load_file(stdPath.c_str());

		if(parseResult.status != status_ok)
			CM_EXCEPT(InternalErrorException, "XML parsing failed: " + toString(parseResult.description()));

		xml_node camelotEditor = xmldoc.child("CamelotEditor");
		xml_node recentlyUsedProjects = camelotEditor.child("RecentlyUsedProjects");

		for(auto iter = recentlyUsedProjects.begin(); iter != recentlyUsedProjects.end(); ++iter)
		{
			xml_attribute pathAttrib = iter->attribute("path");
			mRecentlyUsedProjects.push_back(pathAttrib.value());
		}

		xml_node lastUsedProjectDirectory = camelotEditor.child("LastUsedProjectDir");
		xml_attribute pathAttrib = lastUsedProjectDirectory.attribute("path");
		mLastUsedProjectDirectory = pathAttrib.value();
	}

	void EditorPrefs::clear()
	{
		mRecentlyUsedProjects.clear();
		mLastUsedProjectDirectory = "";
	}

	EditorPrefs& gEditorPrefs()
	{
		return EditorPrefs::instance();
	}
}