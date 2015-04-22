#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Utility class for dealing with files.
	 */
	class BS_UTILITY_EXPORT FileSystem
	{
	public:
		/**
		 * @brief	Opens a file and returns a data stream capable of reading or writing to that file.
		 *
		 * @param	fullPath	Full path to a file.
		 * @param	readOnly	(optional) If true, returned stream will only be readable.
		 */
		static DataStreamPtr openFile(const Path& fullPath, bool readOnly = true);

		/**
		 * @brief	Opens a file and returns a data stream capable of reading and writing to that file.
		 * 			If file doesn't exist new one will be created.
		 *
		 * @param	fullPath	Full path to a file.
		 */
		static DataStreamPtr createAndOpenFile(const Path& fullPath);

		/**
		 * @brief	Returns the size of a file in bytes.
		 *
		 * @param	fullPath	Full path to a file.
		 */
		static UINT64 getFileSize(const Path& fullPath);

		/**
		 * @brief	Deletes a file or a folder at the specified path.
		 *
		 * @param	fullPath   	Full path to a file or a folder..
		 * @param	recursively	(optional) If true, folders will have their contents
		 * 						deleted as well. Otherwise an exception will be
		 * 						thrown for non-empty folders.
		 */
		static void remove(const Path& fullPath, bool recursively = true);

		/**
		 * @brief	Moves a file or a folder from one to another path. This
		 * 			can also be used as a rename operation.
		 *
		 * @param	oldPath			 	Full path to the old file/folder.
		 * @param	newPath			 	Full path to the new file/folder.
		 * @param	overwriteExisting	(optional) If true, any existing file/folder at the new location will be overwritten,
		 * 								otherwise an exception will be thrown if a file/folder already exists.
		 */
		static void move(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

		/**
		 * @brief	Makes a copy of a file or a folder in the specified path. 
		 *
		 * @param	oldPath			 	Full path to the old file/folder.
		 * @param	newPath			 	Full path to the new file/folder.
		 * @param	overwriteExisting	(optional) If true, any existing file/folder at the new location will be overwritten,
		 * 								otherwise an exception will be thrown if a file/folder already exists.
		 */
		static void copy(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

		/**
		 * @brief	Creates a folder at the specified path.
		 *
		 * @param	fullPath	Full path to a full folder to create.
		 */
		static void createDir(const Path& fullPath);

		/**
		 * @brief	Returns true if a file or a folder exists at the specified path.
		 *
		 * @param	fullPath	Full path to a file or folder.
		 */
		static bool exists(const Path& fullPath);

		/**
		 * @brief	Returns true if a file exists at the specified path.
		 *
		 * @param	fullPath	Full path to a file or folder.
		 */
		static bool isFile(const Path& fullPath);

		/**
		 * @brief	Returns true if a folder exists at the specified path.
		 *
		 * @param	fullPath	Full path to a file or folder.
		 */
		static bool isDirectory(const Path& fullPath);

		/**
		 * @brief	Returns all files or folders located in the specified folder.
		 *
		 * @param	dirPath			   	Full path to the folder to retrieve children files/folders from.
		 * @param [out]	files	   		Full paths to all files located directly in specified folder.
		 * @param [out]	directories		Full paths to all folders located directly in specified folder.
		 */
		static void getChildren(const Path& dirPath, Vector<Path>& files, Vector<Path>& directories);

		/**
		 * @brief	Iterates over all files and directories in the specified folder and calls the provided callback
		 *			when a file/folder is iterated over.
		 *
		 * @param	dirPath			Directory over which to iterate
		 * @param	fileCallback	Callback to call whenever a file is found. If callback returns false iteration stops. Can be null.
		 * @param	dirCallback		Callback to call whenever a directory is found. If callback returns false iteration stops. Can be null.
		 * @param	recursive		If false then only the direct children of the provided folder
		 *							will be iterated over, and if true then child directories will
		 *							be recursively visited as well.
		 *
		 * @returns	True if iteration finished iterating over all files/folders, or false if it was interrupted
		 *			by a callback returning false.
		 */
		static bool iterate(const Path& dirPath, std::function<bool(const Path&)> fileCallback,
			std::function<bool(const Path&)> dirCallback = nullptr, bool recursive = true);

		/**
		 * @brief	Returns the last modified time of a file or a folder at the specified path.
		 *
		 * @param	fullPath	Full path to a file or a folder.
		 */
		static std::time_t getLastModifiedTime(const Path& fullPath);

		/**
		 * @brief	Returns the path to the currently working directory.
		 */
		static Path getWorkingDirectoryPath();
	};
}