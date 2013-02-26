#include "CmProjectPrefs.h"
#include "CmFileSystem.h"
#include "3rdParty/pugixml/pugixml.hpp"

using namespace pugi;

namespace CamelotEditor
{
	const QString& ProjectPrefs::getProjectName() const
	{
		return mName;
	}

	void ProjectPrefs::setProjectName(const QString& name)
	{
		mName = name;
	}

	void ProjectPrefs::save(const QString& path, bool overwrite) const
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
		xml_node camelotEditor = xmldoc.append_child("CamelotProject");

		xml_attribute nameAttrib = camelotEditor.append_attribute("name");
		nameAttrib.set_value(mName.toStdString().c_str());

		xmldoc.save_file(stdPath.c_str());
	}

	void ProjectPrefs::load(const QString& path)
	{
		clear();

		String stdPath = path.toStdString();

		if(!FileSystem::fileExists(stdPath))
			CM_EXCEPT(FileNotFoundException, "Specified file: " + stdPath + " does not exist.");

		xml_document xmldoc;
		xml_parse_result parseResult = xmldoc.load_file(stdPath.c_str());

		if(parseResult.status != status_ok)
			CM_EXCEPT(InternalErrorException, "XML parsing failed: " + toString(parseResult.description()));

		xml_node camelotProject = xmldoc.child("CamelotProject");
		xml_attribute nameAttrib = camelotProject.attribute("name");
		mName = nameAttrib.value();
	}

	void ProjectPrefs::clear()
	{
		mName = "";
	}

	ProjectPrefs& gProjectPrefs()
	{
		return ProjectPrefs::instance();
	}
}