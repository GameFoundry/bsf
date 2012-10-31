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
		static String getExtension(const String& path)
		{
			return boost::filesystem::extension(path);
		}

		static bool hasExtension(const String& path, const String& extension)
		{
			return getExtension(path) == extension;
		}

		static String combine(const String& base, const String& name)
		{
			if (base.empty())
				return name;
			else
				return base + '/' + name;
		}
	};
}