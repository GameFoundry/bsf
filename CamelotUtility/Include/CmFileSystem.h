#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT FileSystem
	{
	public:
		static DataStreamPtr openFile(const WString& fullPath, bool readOnly = true);
		static DataStreamPtr createAndOpenFile(const WString& fullPath);

		static UINT64 getFileSize(const WString& fullPath);

		static void remove(const WString& fullPath, bool recursively = true);
		static void move(const WString& oldPath, const WString& newPath, bool overwriteExisting = true);

		static void createDir(const WString& fullPath);

		static bool exists(const WString& fullPath);
		static bool isFile(const WString& fullPath);
		static bool isDirectory(const WString& fullPath);

		static void getChildren(const WString& dirPath, Vector<WString>::type& files, Vector<WString>::type& directories);

		static std::time_t getLastModifiedTime(const WString& fullPath);

		static WString getWorkingDirectoryPath();
		static WString getParentDirectory(const WString& path);
	};
}