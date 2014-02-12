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
		static WString getExtension(const WString& path)
		{
			boost::filesystem3::wpath ext = boost::filesystem3::extension(boost::filesystem3::wpath(path.c_str()));
			return ext.wstring().c_str();
		}

		static bool hasExtension(const WString& path, const WString& extension)
		{
			return getExtension(path) == extension;
		}

		static WString combine(const WString& base, const WString& name)
		{
			if (base.empty())
				return name;
			else
				return base + L'/' + name;
		}

		/**
		 * @brief	Method for standardizing paths - use forward slashes only, end with slash.
		 */
		static WString standardisePath(const WString& inPath)
		{
			WString path = inPath;

			std::replace(path.begin(), path.end(), L'\\', L'/');
			if(path[path.length() - 1] != L'/')
				path += L'/';

			return path;
		}

		/**
		 * @brief	Method for splitting a fully qualified filename into the base name and path.
		 */
		static void splitFilename(const WString& qualifiedName, WString& outBasename, WString& outPath)
		{
			WString path = qualifiedName;
			// Replace \ with / first
			std::replace( path.begin(), path.end(), L'\\', L'/' );
			// split based on final /
			size_t i = path.find_last_of(L'/');

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
		static void splitBaseFilename(const WString& fullName, WString& outBasename, WString& outExtension)
		{
			size_t i = fullName.find_last_of(L".");
			if (i == CamelotFramework::WString::npos)
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
		static void splitFullFilename(const WString& qualifiedName, WString& outBasename, WString& outExtention, WString& outPath)
		{
			WString fullName;
			splitFilename(qualifiedName, fullName, outPath);
			splitBaseFilename(fullName, outBasename, outExtention);
		}
	};
}