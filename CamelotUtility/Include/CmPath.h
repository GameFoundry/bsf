#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/filesystem.hpp>

namespace BansheeEngine
{
	/**
	 * @brief	Class for storing and manipulating file paths. Paths may be parsed
	 *			from and to raw strings according to various platform specific path
	 *			types.
	 */
	class CM_UTILITY_EXPORT Path
	{
	public:
		enum class PathType
		{
			Windows,
			Unix,
			Default
		};

	public:
		/**
		 * @brief	Constructs a path by parsing the provided path string. 
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		Path(const WString& pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		Path(const String& pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path null terminated string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		Path(wchar_t* pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path null terminated string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		Path(const char* pathStr, PathType type = PathType::Default);
		Path(const Path& other);

		/**
		 * @brief	Assigns a path by parsing the provided path string. Path will be 
		 *			parsed according to the rules of the platform the application is 
		 *			being compiled to.
		 */
		Path& operator= (const WString& pathStr);

		/**
		 * @brief	Assigns a path by parsing the provided path string. Path will be
		 *			parsed according to the rules of the platform the application is
		 *			being compiled to.
		 */
		Path& operator= (const String& pathStr);

		/**
		 * @brief	Assigns a path by parsing the provided path null terminated string. 
		 *			Path will be parsed according to the rules of the platform the 
		 *			application is being compiled to.
		 */
		Path& operator= (const wchar_t* pathStr);

		/**
		 * @brief	Assigns a path by parsing the provided path null terminated string.
		 *			Path will be parsed according to the rules of the platform the
		 *			application is being compiled to.
		 */
		Path& operator= (const char* pathStr);

		Path& operator= (const Path& path);

		/**
		 * @brief	Compares two paths and returns true if they match. Comparison is
		 *			case insensitive and paths will be compared as-is, without canonization.
		 */
		bool operator== (const Path& path) const { return equals(path); }

		/**
		 * @brief	Compares two paths and returns true if they don't match. Comparison is
		 *			case insensitive and paths will be compared as-is, without canonization.
		 */
		bool operator!= (const Path& path) const { return !equals(path); }

		/**
		* @brief	Gets a directory name with the specified index from the path.
		*/
		const WString& operator[] (UINT32 idx) const { return getWDirectory(idx); }

		/**
		 * @brief	Swap internal data with another Path object.
		 */
		void swap(Path& path);

		/**
		 * @brief	Create a path from another Path object.
		 */
		void assign(const Path& path);

		/**
		 * @brief	Constructs a path by parsing the provided path string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		void assign(const WString& pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		void assign(const String& pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path null terminated string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		void assign(const wchar_t* pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Constructs a path by parsing the provided path null terminated string.
		 *			Throws exception if provided path is not valid.
		 *
		 * @param	type	If set to default path will be parsed according to the
		 *					rules of the platform the application is being compiled to.
		 *					Otherwise it will be parsed according to provided type.
		 */
		void assign(const char* pathStr, PathType type = PathType::Default);

		/**
		 * @brief	Converts the path in a string according to platform path rules.
		 *
		 * @type	If set to default path will be parsed according to the
		 *			rules of the platform the application is being compiled to.
		 *			Otherwise it will be parsed according to provided type.
		 */
		WString toWString(PathType type = PathType::Default) const;

		/**
		 * @brief	Converts the path in a string according to platform path rules.
		 *
		 * @type	If set to default path will be parsed according to the
		 *			rules of the platform the application is being compiled to.
		 *			Otherwise it will be parsed according to provided type.
		 */
		String toString(PathType type = PathType::Default) const;

		/**
		 * @brief	Checks is the path a directory (contains no file-name).
		 */
		bool isDirectory() const { return mFilename.empty(); }

		/**
		 * @brief	Checks does the path point to a file.
		 */
		bool isFile() const { return !mFilename.empty(); }

		/**
		 * @brief	Checks is the contained path absolute.
		 */
		bool isAbsolute() const { return mIsAbsolute; }

		/**
		 * @brief	Returns parent path. If current path points to a file
		 *			the parent path will be the folder where the file is located.
		 *			Or if it contains a directory the parent will be the parent directory.
		 *			If no parent exists, same path will be returned.
		 */
		Path getParent() const;

		/**
		 * @brief	Returns an absolute path by appending the current path to the provided base.
		 *			If path was already absolute no changes are made and copy of current path
		 *			is returned.
		 *			If base is not absolute, then the returned path will be made relative to base,
		 *			but will not be absolute.
		 */
		Path getAbsolute(const Path& base) const;

		/**
		 * @brief	Returns a relative path by making the current path relative to the provided base.
		 *			Base must be a part of the current path. If base is not a part
		 *			of the path no changes are made and a copy of the current path
		 *			is returned.
		 */
		Path getRelative(const Path& base) const;

		/**
		 * @brief	Returns the path as a path to directory. If path was pointing
		 *			to a file, the filename is removed, otherwise no changes are made
		 *			and exact copy is returned.
		 */
		Path getDirectory() const;

		/**
		 * @brief	Makes the path the parent of the current path. If current path points to a file
		 *			the parent path will be the folder where the file is located.
		 *			Or if it contains a directory the parent will be the parent directory.
		 *			If no parent exists, same path will be returned.
		 */
		void makeParent();

		/**
		 * @brief	Makes the current path absolute by appending it to base.
		 *			If path was already absolute no changes are made and copy of current path
		 *			is returned.
		 *			If base is not absolute, then the returned path will be made relative to base,
		 *			but will not be absolute.
		 */
		void makeAbsolute(const Path& base);

		/**
		 * @brief	Makes the current path relative to the provided base.
		 *			Base must be a part of the current path. If base is not a part
		 *			of the path no changes are made and a copy of the current path
		 *			is returned.
		 */
		void makeRelative(const Path& base);

		/**
		 * @brief	Appends another path to the end of this path.
		 *			If this path contains a filename, it is removed.
		 */
		void append(const Path& path);

		/**
		 * @brief	Checks if the current path contains the provided path.
		 *			Comparison is case insensitive and paths will be compared 
		 *			as-is, without canonization.
		 */
		bool includes(const Path& child) const;

		/**
		 * @brief	Compares two paths and returns true if they match. Comparison is
		 *			case insensitive and paths will be compared as-is, without canonization.
		 */
		bool equals(const Path& other) const;

		/**
		 * @brief	Change or set the filename in the path.
		 */
		void setFilename(const WString& filename) { mFilename = filename; }

		/**
		 * @brief	Change or set the filename in the path.
		 */
		void setFilename(const String& filename) { mFilename = BansheeEngine::toWString(filename); }

		/**
		 * @brief	Change or set the base name in the path. Base name changes the
		 *			filename by changing its base to the provided value but keeping extension intact.
		 */
		void setBasename(const WString& basename);

		/**
		 * @brief	Change or set the base name in the path. Base name changes the
		 *			filename by changing its base to the provided value but keeping extension intact.
		 */
		void setBasename(const String& basename);

		/**
		 * @brief	Change or set the extension of the filename in the path.
		 */
		void setExtension(const WString& extension);

		/**
		 * @brief	Change or set the extension of the filename in the path.
		 */
		void setExtension(const String& extension);

		/**
		 * @brief	Returns a filename in the path.
		 *
		 * @param	extension	If true, returned filename will contain an extension.
		 */
		WString getWFilename(bool extension = true) const;

		/**
		 * @brief	Returns a filename in the path.
		 *
		 * @param	extension	If true, returned filename will contain an extension.
		 */
		String getFilename(bool extension = true) const;

		/**
		 * @brief	Returns file extension with the leading ".".
		 */
		WString getWExtension() const;

		/**
		 * @brief	Returns file extension with the leading ".".
		 */
		String getExtension() const;

		/**
		 * @brief	Gets the number of directories in the path.
		 */
		UINT32 getNumDirectories() const { return (UINT32)mDirectories.size(); }

		/**
		 * @brief	Gets a directory name with the specified index from the path.
		 */
		const WString& getWDirectory(UINT32 idx) const;

		/**
		* @brief	Gets a directory name with the specified index from the path.
		*/
		String getDirectory(UINT32 idx) const;

		/**
		 * @brief	Returns path device (e.g. drive, volume, etc.) if one exists in the path.
		 */
		const WString& getWDevice() const { return mDevice; }

		/**
		* @brief	Returns path device (e.g. drive, volume, etc.) if one exists in the path.
		*/
		String getDevice() const { return BansheeEngine::toString(mDevice); }

		/**
		* @brief	Returns path node (e.g. network name) if one exists in the path.
		*/
		const WString& getWNode() const { return mNode; }

		/**
		* @brief	Returns path node (e.g. network name) if one exists in the path.
		*/
		String getNode() const { return BansheeEngine::toString(mNode); }

		/**
		 * @brief	Clears the path to nothing.
		 */
		void clear();

	private:
		Path();

		/**
		* @brief	Constructs a path by parsing the provided raw string data.
		*			Throws exception if provided path is not valid.
		*
		* @param	type	If set to default path will be parsed according to the
		*					rules of the platform the application is being compiled to.
		*					Otherwise it will be parsed according to provided type.
		*/
		void assign(const wchar_t* pathStr, UINT32 numChars, PathType type = PathType::Default);

		/**
		* @brief	Constructs a path by parsing the provided raw string data.
		*			Throws exception if provided path is not valid.
		*
		* @param	type	If set to default path will be parsed according to the
		*					rules of the platform the application is being compiled to.
		*					Otherwise it will be parsed according to provided type.
		*/
		void assign(const char* pathStr, UINT32 numChars, PathType type = PathType::Default);

		/**
		 * @brief	Parses a Windows path and stores the parsed data internally.
		 *			Throws an exception if parsing fails.
		 */
		template<class T>
		void parseWindows(const T* pathStr, UINT32 numChars)
		{
			clear();

			UINT32 idx = 0;
			BasicStringStream<T>::type tempStream;

			if (idx < numChars)
			{
				if (pathStr[idx] == '\\' || pathStr[idx] == '/')
				{
					mIsAbsolute = true;
					idx++;
				}
			}

			if (idx < numChars)
			{
				// Path starts with a node, a drive letter or is relative
				if (mIsAbsolute && pathStr[idx] == '\\' || pathStr[idx] == '/') // Node
				{
					idx++;

					tempStream.clear();
					while (idx < numChars && pathStr[idx] != '\\' && pathStr[idx] != '/')
						tempStream << pathStr[idx++];

					setNode(tempStream.str());

					if (idx < numChars)
						idx++;
				}
				else // A drive letter or not absolute
				{
					T drive = pathStr[idx];
					idx++;

					if (idx < numChars && pathStr[idx] == ':')
					{
						if (mIsAbsolute || !((drive >= 'a' && drive <= 'z') || (drive >= 'A' && drive <= 'Z')))
							throwInvalidPathException(BasicString<T>::type(pathStr, numChars));

						mIsAbsolute = true;
						setDevice(BansheeEngine::toWString(drive));

						idx++;

						if (idx < numChars || (pathStr[idx] != '\\' && pathStr[idx] != '/'))
							throwInvalidPathException(BasicString<T>::type(pathStr, numChars));

						idx++;
					}
					else
						idx--;
				}

				while (idx < numChars)
				{
					tempStream.clear();
					while (idx < numChars && pathStr[idx] != '\\' && pathStr[idx] != '/')
					{
						tempStream << pathStr[idx];
						idx++;
					}

					if (idx < numChars)
						pushDirectory(tempStream.str());
					else
						setFilename(tempStream.str());

					idx++;
				}
			}
		}

		/**
		* @brief	Parses a Unix path and stores the parsed data internally.
		*			Throws an exception if parsing fails.
		*/
		template<class T>
		void parseUnix(const T* pathStr, UINT32 numChars)
		{
			clear();

			UINT32 idx = 0;
			BasicStringStream<T>::type tempStream;

			if (idx < numChars)
			{
				if (pathStr[idx] == '/')
				{
					mIsAbsolute = true;
					idx++;
				}
				else if (pathStr[idx] == '~')
				{
					idx++;
					if (idx >= numChars || pathStr[idx] == '/')
					{
						pushDirectory(BansheeEngine::toWString('~'));
						mIsAbsolute = true;
					}
					else
						idx--;
				}

				while (idx < numChars)
				{
					tempStream.clear();
					while (idx < numChars && pathStr[idx] != '/')
					{
						tempStream << pathStr[idx];
						idx++;
					}

					if (idx < numChars)
					{
						if (mDirectories.empty())
						{
							BasicString<T>::type deviceStr = tempStream.str();
							if (!deviceStr.empty() && *(deviceStr.rbegin()) == ':')
							{
								setDevice(deviceStr.substr(0, deviceStr.length() - 1));
								mIsAbsolute = true;
							}
							else
							{
								pushDirectory(deviceStr);
							}
						}
						else
						{
							pushDirectory(tempStream.str());
						}
					}
					else
					{
						setFilename(tempStream.str());
					}

					idx++;
				}
			}
		}

		void setNode(const WString& node) { mNode = node; }
		void setNode(const String& node) { mNode = BansheeEngine::toWString(node); }

		void setDevice(const WString& device) { mDevice = device; }
		void setDevice(const String& device) { mDevice = BansheeEngine::toWString(device); }

		/**
		 * @brief	Build a Windows path string from internal path data.
		 */
		WString buildWindows() const;

		/**
		* @brief	Build a Unix path string from internal path data.
		*/
		WString buildUnix() const;

		/**
		 * @brief	Add new directory to the end of the path. 
		 */
		void pushDirectory(const WString& dir);

		/**
		* @brief	Add new directory to the end of the path.
		*/
		void pushDirectory(const String& dir);

		/**
		 * @brief	Compares two path elements (i.e. filenames, directory names, etc.)
		 */
		bool comparePathElem(const WString& left, const WString& right) const;

		/**
		 * @brief	Helper method that throws invalid path exception. 
		 */
		void throwInvalidPathException(const WString& path) const;

		/**
		* @brief	Helper method that throws invalid path exception.
		*/
		void throwInvalidPathException(const String& path) const;
	private:
		Vector<WString>::type mDirectories;
		WString mDevice;
		WString mFilename;
		WString mNode;
		bool mIsAbsolute;
	};

	/**
	 * @brief	Various string manipulations of file paths.
	 */
	class CM_UTILITY_EXPORT OldPath
	{
	public:
		/**
		 * @brief	Returns file extension extracted from the provided
		 * 			path, with a leading ".".
		 */
		static WString getExtension(const WString& path)
		{
			boost::filesystem3::wpath ext = boost::filesystem3::extension(boost::filesystem3::wpath(path.c_str()));
			return ext.wstring().c_str();
		}

		/**
		 * @brief	Query if a path has the specified extension. Provided
		 * 			extension must contain the leading ".".
		 */
		static bool hasExtension(const WString& path, const WString& extension)
		{
			return getExtension(path) == extension;
		}

		/**
		 * @brief	Replaces or adds an extension on a file path. Provided
		 * 			extension must contain the leading ".".
		 */
		static void replaceExtension(WString& path, const WString& newExtension)
		{
			boost::filesystem3::path internalPath = path.c_str();

			path = internalPath.replace_extension(newExtension.c_str()).c_str();
		}

		/**
		 * @brief	Returns a path that is one level higher than the provided path, unless the path
		 * 			is already at the root. Otherwise returns the initial path.
		 */
		static WString parentPath(const WString& path)
		{
			boost::filesystem3::path internalPath = path.c_str();

			return internalPath.parent_path().c_str();
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
				if(iterChild == childPathElems.end())
					return false;

				if(!comparePathElements(*iterChild, *iterParent))
					return false;
			}

			return true;
		}

		/**
		 * @brief	Returns path relative to base.
		 */
		static WString relative(const WString& base, const WString& path)
		{
			Vector<WString>::type basePathElems = split(base);
			Vector<WString>::type pathElems = split(path);

			auto iterBase = basePathElems.begin();
			auto iterPath = pathElems.begin();

			for(; iterBase != basePathElems.end(); ++iterBase, ++iterPath)
			{
				if(!comparePathElements(*iterBase, *iterPath))
					return L"";
			}

			WString relativePath;
			for(; iterPath != pathElems.end(); ++iterPath)
			{
				relativePath = OldPath::combine(relativePath, *iterPath);
			}

			return relativePath;
		}

		/**
		 * @brief	Splits a path into string entries. Path separator
		 * 			may be "\" or "/". Path separator will not be included
		 * 			in the returned strings.
		 */
		static Vector<WString>::type split(const WString& path)
		{
			Vector<WString>::type splitPath;

			WString standardizedPath = standardizePath(path);
			return StringUtil::split(standardizedPath, L"/");
		}

		/**
		 * @brief	Combines two paths using the "/" path separator.
		 */
		static WString combine(const WString& base, const WString& name)
		{
			if (base.empty())
				return name;
			else
				return base + L'/' + name;
		}

		/**
		 * @brief	Compares two canonical paths and returns true if they are equal.
		 */
		static bool equals(const WString& left, const WString& right)
		{
			Vector<WString>::type leftElements = split(left);
			Vector<WString>::type rightElements = split(right);

			UINT32 idx = 0;
			for(auto& leftElem : leftElements)
			{
				if(leftElem.empty())
					continue;
				
				while(idx < (UINT32)rightElements.size() && rightElements[idx].empty())
					idx++;

				if(idx >= (UINT32)rightElements.size())
					return false; // Right path is deeper than left path

				if(!comparePathElements(leftElem, rightElements[idx]))
					return false;

				idx++;
			}

			while(idx < (UINT32)rightElements.size() && rightElements[idx].empty())
				idx++;

			if(idx < (UINT32)rightElements.size())
				return false; // Left path is deeper than right path

			return true;
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
		 * @brief	Extracts filename from the provided path.
		 *
		 * @param	path				Path to the file.
		 * @param	includeExtension	(optional) If true, filename extension will be included in the returned string.
		 */
		static WString getFilename(const WString& path, bool includeExtension = true)
		{
			boost::filesystem3::path internalPath = path.c_str();
			
			if(includeExtension)
				return internalPath.filename().c_str();
			else
				return internalPath.stem().c_str();
		}

		/**
		 * @brief	Method for standardizing paths - use forward slashes only, end without a slash.
		 */
		static WString standardizePath(const WString& inPath)
		{
			WString path = inPath;

			std::replace(path.begin(), path.end(), L'\\', L'/');

			while(path.length() > 0 && path.back() == L'/')
				path.pop_back();

			return path;
		}
	};
}