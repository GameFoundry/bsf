#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT FileSystem
	{
	public:
		static DataStreamPtr open(const String& fullPath, bool readOnly = true);

		static DataStreamPtr create(const String& fullPath);

		static void remove(const String& fullPath);

		static bool fileExists(const String& fullPath);
		static bool dirExists(const String& fullPath);

		static void createDir(const String& fullPath);
		static void deleteDir(const String& fullPath);

		static vector<String>::type getFiles(const String& dirPath);

		static String getCurrentPath();

		static bool isValidFileName(const String& name);

		static String getDirectoryPath(const String& path);
	};
}