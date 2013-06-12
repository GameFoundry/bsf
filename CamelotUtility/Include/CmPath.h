#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/filesystem.hpp>

namespace CamelotFramework
{
	/**
	 * @brief	Various string manipulations of file paths.
	 */
	class Path
	{
	public:
		static String getExtension(const String& path)
		{
			return boost::filesystem::extension(boost::filesystem::path(path.c_str())).c_str();
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

		/**
		 * @brief	Method for standardizing paths - use forward slashes only, end with slash.
		 */
		String standardisePath(const String& inPath)
		{
			String path = inPath;

			std::replace(path.begin(), path.end(), '\\', '/');
			if(path[path.length() - 1] != '/')
				path += '/';

			return path;
		}

		/**
		 * @brief	Method for splitting a fully qualified filename into the base name and path.
		 */
		void splitFilename(const String& qualifiedName, String& outBasename, String& outPath)
		{
			String path = qualifiedName;
			// Replace \ with / first
			std::replace( path.begin(), path.end(), '\\', '/' );
			// split based on final /
			size_t i = path.find_last_of('/');

			if (i == String::npos)
			{
				outPath.clear();
				outBasename = qualifiedName;
			}
			else
			{
				outBasename = path.substr(i+1, path.size() - i - 1);
				outPath = path.substr(0, i+1);
			}
		}

		/**
		 * @brief	Method for splitting a filename into the base name and extension.
		 */
		void splitBaseFilename(const String& fullName, String& outBasename, String& outExtension)
		{
			size_t i = fullName.find_last_of(".");
			if (i == CamelotFramework::String::npos)
			{
				outExtension.clear();
				outBasename = fullName;
			}
			else
			{
				outExtension = fullName.substr(i+1);
				outBasename = fullName.substr(0, i);
			}
		}

		/**
		 * @brief	Method for splitting a fully qualified filename into the base name, extension and path.
		 */
		void splitFullFilename(const String& qualifiedName, String& outBasename, String& outExtention, String& outPath)
		{
			String fullName;
			splitFilename(qualifiedName, fullName, outPath);
			splitBaseFilename(fullName, outBasename, outExtention);
		}
	};
}