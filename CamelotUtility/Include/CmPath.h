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

		static void replaceExtension(WString& path, const WString& newExtension)
		{
			path = toWString(toPath(path).replace_extension(newExtension.c_str()));
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
			Vector<WString>::type childPathElems = split(child);
			Vector<WString>::type parentPathElems = split(parent);

			auto iterChild = childPathElems.begin();
			auto iterParent = parentPathElems.begin();

			for(; iterParent != parentPathElems.end(); ++iterChild, ++iterParent)
			{
				if(!comparePathElements(*iterChild, *iterParent))
					return false;
			}

			return true;
		}

		static Vector<WString>::type split(const WString& path)
		{
			Vector<WString>::type splitPath;

			WString standardizedPath = standardisePath(path);
			return StringUtil::split(standardizedPath, L"/");
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

		/**
		 * @brief	Compares two path elements for equality. 
		 * 			
		 * @note	Should not be used for comparing entire paths. First you need to split your
		 * 			path into sub-elements using some other method and then send those sub-elements to
		 * 			this method. 
		 */
		static bool comparePathElements(const WString& left, const WString& right)
		{
			if(left.size() != right.size())
				return false;

			for(UINT32 i = 0; i < (UINT32)left.size(); i++)
			{
#if CM_PLATFORM == CM_PLATFORM_WIN32 // Compare case insensitive
				if(tolower(left[i]) != tolower(right[i]))
					return false;
#else
				assert(false); // Implement case sensitive or insensitive comparison, depending on platform
#endif
			}

			return true;
		}

		/**
		 * @see		comparePathElements(const WString&, const WString&)
		 */
		static bool comparePathElements(const WPath& left, const WPath& right)
		{
			if(left.native().size() != right.native().size())
				return false;

			for(UINT32 i = 0; i < (UINT32)left.native().size(); i++)
			{
#if CM_PLATFORM == CM_PLATFORM_WIN32 // Compare case insensitive
				if(tolower(left.native()[i]) != tolower(right.native()[i]))
					return false;
#else
				assert(false); // Implement case sensitive or insensitive comparison, depending on platform
#endif
			}

			return true;
		}

		static WString getFilename(const WString& path)
		{
			return toWString(toPath(path).filename());
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

			if(path.length() > 0)
			{
				if(path[path.length() - 1] != L'/')
					path += L'/';
			}

			return path;
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