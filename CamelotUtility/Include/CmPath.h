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
	};
}