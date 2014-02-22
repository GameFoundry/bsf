#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/filesystem.hpp>

namespace CamelotFramework
{
	typedef boost::filesystem3::path Path;
	typedef boost::filesystem3::wpath WPath;

	Path CM_UTILITY_EXPORT toPath(const String& p);
	WPath CM_UTILITY_EXPORT toPath(const WString& p);

	String CM_UTILITY_EXPORT toString(const Path& p);
	WString CM_UTILITY_EXPORT toWString(const WPath& p);

	/**
	 * @brief	Various string manipulations of file paths.
	 */
	class CM_UTILITY_EXPORT PathUtil
	{
	public:
		static const WPath BLANK;

		static WString getExtension(const WString& path)
		{
			boost::filesystem3::wpath ext = boost::filesystem3::extension(boost::filesystem3::wpath(path.c_str()));
			return ext.wstring().c_str();
		}

		static WPath getExtension(const WPath& path)
		{
			return boost::filesystem3::extension(path);
		}

		static bool hasExtension(const WString& path, const WString& extension)
		{
			return getExtension(path) == extension;
		}

		static void replaceExtension(WPath& path, const WString& newExtension)
		{
			path.replace_extension(newExtension.c_str());
		}

		static WString parentPath(const WString& path)
		{
			return WPath(path.c_str()).parent_path().c_str();
		}

		static WPath parentPath(const WPath& path)
		{
			return path.parent_path();
		}

		/**
		 * @brief	Returns true if path child is included in path parent.
		 * 			Both paths must be canonical.
		 */
		static bool includes(const WString& child, const WString& parent)
		{
			boost::filesystem3::wpath childPath = child.c_str();
			boost::filesystem3::wpath parentPath = parent.c_str();

			auto iterChild = childPath.begin();
			auto iterParent = parentPath.begin();

			for(; iterChild != childPath.end(); ++iterChild, ++iterParent)
			{
				if(*iterChild != *iterParent)
					return false;
			}

			return true;
		}

		static WString combine(const WString& base, const WString& name)
		{
			if (base.empty())
				return name;
			else
				return base + L'/' + name;
		}

		static WPath combine(const WPath& base, const WPath& name)
		{
			return base / name;
		}

		static WPath getFilename(const WPath& path)
		{
			return path.filename();
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

	template<> struct RTTIPlainType<WPath>
	{	
		enum { id = TID_WPath }; enum { hasDynamicSize = 1 };

		static void toMemory(const WPath& data, char* memory)
		{ 
			rttiWriteElem(toWString(data), memory);
		}

		static UINT32 fromMemory(WPath& data, char* memory)
		{ 
			WString strData;

			RTTIPlainType<WString>::fromMemory(strData, memory);
			data = toPath(strData);

			return rttiGetElemSize(strData);
		}

		static UINT32 getDynamicSize(const WPath& data)	
		{ 
			return rttiGetElemSize(toWString(data));
		}	
	}; 
}