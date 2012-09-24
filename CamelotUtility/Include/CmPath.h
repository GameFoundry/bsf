#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/filesystem.hpp>

namespace CamelotEngine
{
	/**
	 * @brief	Various utility methods for handling file paths.
	 */
	class Path
	{
	public:
		static bool exists(const String& path)
		{
			return boost::filesystem::exists(path);
		}

		static String getExtension(const String& path)
		{
			return boost::filesystem::extension(path);
		}


		static bool isAbsolute(const String& path)
		{
#if CM_PLATFORM == CM_PLATFORM_WIN32
			if (isalpha(UINT8(path[0])) && path[1] == ':')
				return true;
#endif
			return path[0] == '/' || path[0] == '\\';
		}

		static String concatenatePath(const String& base, const String& name)
		{
			if (base.empty() || isAbsolute(name.c_str()))
				return name;
			else
				return base + '/' + name;
		}
	};
}