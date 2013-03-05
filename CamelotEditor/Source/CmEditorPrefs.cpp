#include "CmEditorPrefs.h"
#include "CmFileSystem.h"
#include <QtCore/QDir>

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

	void EditorPrefs::setMainWindowLayout(const WindowLayoutDesc& desc)
	{
		mMainWindowLayout = desc;
		mMainWindowLayout.name = "MainWindow";
	}

	const WindowLayoutDesc& EditorPrefs::getMainWindowLayout() const
	{
		return mMainWindowLayout;
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

		xml_node openWindows = camelotEditor.append_child("OpenWindows");
		saveWindowLayout(openWindows, mMainWindowLayout);

		for(auto iter = mWindowLayouts.begin(); iter != mWindowLayouts.end(); ++iter)
		{
			saveWindowLayout(openWindows, *iter);
		}

		xmldoc.save_file(stdPath.c_str());
	}

	void EditorPrefs::saveWindowLayout(xml_node parentNode, const WindowLayoutDesc& desc) const
	{
		xml_node windowLayout = parentNode.append_child("WindowLayout");
		windowLayout.append_attribute("name").set_value(desc.name.toStdString().c_str());

		xml_node windowGeometry = windowLayout.append_child("Geometry");
		windowGeometry.append_attribute("left").set_value(desc.left);
		windowGeometry.append_attribute("top").set_value(desc.top);
		windowGeometry.append_attribute("width").set_value(desc.width);
		windowGeometry.append_attribute("height").set_value(desc.height);

		xml_node windowScreen = windowLayout.append_child("Screen");
		windowScreen.append_attribute("fullscreen").set_value(desc.maximized);
		windowScreen.append_attribute("screenIdx").set_value(desc.screenIdx);

		xml_node dockInfo = windowLayout.append_child("DockInfo");
		dockInfo.append_attribute("state").set_value((UINT32)desc.dockState);
		dockInfo.append_attribute("parentName").set_value(desc.dockParentName.toStdString().c_str());
	}

	WindowLayoutDesc EditorPrefs::loadWindowLayout(xml_node node) const
	{
		WindowLayoutDesc desc;

		desc.name = node.attribute("name").value();

		desc.left = node.child("Geometry").attribute("left").as_uint();
		desc.top = node.child("Geometry").attribute("top").as_uint();
		desc.width = node.child("Geometry").attribute("width").as_uint();
		desc.height = node.child("Geometry").attribute("height").as_uint();

		desc.maximized = node.child("Screen").attribute("fullscreen").as_bool();
		desc.screenIdx = node.child("Screen").attribute("screenIdx").as_uint();

		desc.dockState = (WindowDockState)node.child("DockInfo").attribute("state").as_uint();
		desc.dockParentName = node.child("DockInfo").attribute("parentName").as_string();

		return desc;
	}

	void EditorPrefs::setWindowLayouts(const vector<WindowLayoutDesc>::type& descs)
	{
		mWindowLayouts = descs;
	}

	const vector<WindowLayoutDesc>::type& EditorPrefs::getWindowLayouts() const
	{
		return mWindowLayouts;
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

		xml_node openWindows = camelotEditor.child("OpenWindows");

		bool foundMainWindowLayoutDesc = false;
		for (xml_node windowLayout = openWindows.child("WindowLayout"); windowLayout; windowLayout = windowLayout.next_sibling("WindowLayout"))
		{
			WindowLayoutDesc desc = loadWindowLayout(windowLayout);

			if(!foundMainWindowLayoutDesc)
			{
				mMainWindowLayout = desc;
				foundMainWindowLayoutDesc = true;
			}
			else
			{
				mWindowLayouts.push_back(desc);
			}
		}
	}

	void EditorPrefs::clear()
	{
		mRecentlyUsedProjects.clear();
		mLastUsedProjectDirectory = "";
		mMainWindowLayout = WindowLayoutDesc();
		mWindowLayouts.clear();
	}

	EditorPrefs& gEditorPrefs()
	{
		return EditorPrefs::instance();
	}
}