#include "BsFileSystem.h"
#include "BsException.h"
#include "BsDataStream.h"
#include "BsPath.h"
#include <windows.h>

namespace BansheeEngine
{
	void win32_handleError(DWORD error, const WString& path)
	{
		switch (error)
		{
		case ERROR_FILE_NOT_FOUND:
			BS_EXCEPT(FileNotFoundException, "File at path: \"" + toString(path) + "\" not found.");
		case ERROR_PATH_NOT_FOUND:
		case ERROR_BAD_NETPATH:
		case ERROR_CANT_RESOLVE_FILENAME:
		case ERROR_INVALID_DRIVE:
			BS_EXCEPT(FileNotFoundException, "Path \"" + toString(path) + "\" not found.");
		case ERROR_ACCESS_DENIED:
			BS_EXCEPT(IOException, "Access to path \"" + toString(path) + "\" denied.");
		case ERROR_ALREADY_EXISTS:
		case ERROR_FILE_EXISTS:
			BS_EXCEPT(IOException, "File/folder at path \"" + toString(path) + "\" already exists.");
		case ERROR_INVALID_NAME:
		case ERROR_DIRECTORY:
		case ERROR_FILENAME_EXCED_RANGE:
		case ERROR_BAD_PATHNAME:
			BS_EXCEPT(IOException, "Invalid path string: \"" + toString(path) + "\".");
		case ERROR_FILE_READ_ONLY:
			BS_EXCEPT(IOException, "File at path \"" + toString(path) + "\" is read only.");
		case ERROR_CANNOT_MAKE:
			BS_EXCEPT(IOException, "Cannot create file/folder at path: \"" + toString(path) + "\".");
		case ERROR_DIR_NOT_EMPTY:
			BS_EXCEPT(IOException, "Directory at path \"" + toString(path) + "\" not empty.");
		case ERROR_WRITE_FAULT:
			BS_EXCEPT(IOException, "Error while writing a file at path \"" + toString(path) + "\".");
		case ERROR_READ_FAULT:
			BS_EXCEPT(IOException, "Error while reading a file at path \"" + toString(path) + "\".");
		case ERROR_SHARING_VIOLATION:
			BS_EXCEPT(IOException, "Sharing violation at path \"" + toString(path) + "\".");
		case ERROR_LOCK_VIOLATION:
			BS_EXCEPT(IOException, "Lock violation at path \"" + toString(path) + "\".");
		case ERROR_HANDLE_EOF:
			BS_EXCEPT(IOException, "End of file reached for file at path \"" + toString(path) + "\".");
		case ERROR_HANDLE_DISK_FULL:
		case ERROR_DISK_FULL:
			BS_EXCEPT(IOException, "Disk full.");
		case ERROR_NEGATIVE_SEEK:
			BS_EXCEPT(IOException, "Negative seek.");
		default:
			BS_EXCEPT(IOException, "Undefined file system exception.");
		}
	}

	WString win32_getCurrentDirectory()
	{
		DWORD len = GetCurrentDirectoryW(0, NULL);
		if (len > 0)
		{
			wchar_t* buffer = (wchar_t*)bs_alloc(len * sizeof(wchar_t));

			DWORD n = GetCurrentDirectoryW(len, buffer);
			if (n > 0 && n <= len)
			{
				WString result(buffer);
				if (result[result.size() - 1] != '\\')
					result.append(L"\\");

				bs_free(buffer);
				return result;
			}

			bs_free(buffer);
		}

		return StringUtil::WBLANK;
	}

	bool win32_pathExists(const WString& path)
	{
		DWORD attr = GetFileAttributesW(path.c_str());
		if (attr == 0xFFFFFFFF)
		{
			switch (GetLastError())
			{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
			case ERROR_NOT_READY:
			case ERROR_INVALID_DRIVE:
				return false;
			default:
				win32_handleError(GetLastError(), path);
			}
		}
		return true;
	}

	bool win32_isDirectory(const WString& path)
	{
		DWORD attr = GetFileAttributesW(path.c_str());
		if (attr == 0xFFFFFFFF)
			win32_handleError(GetLastError(), path);

		return (attr & FILE_ATTRIBUTE_DIRECTORY) != FALSE;
	}

	bool win32_isDevice(const WString& path)
	{
		WString ucPath = path;
		StringUtil::toUpperCase(ucPath);

		return
			ucPath.compare(0, 4, L"\\\\.\\") == 0 ||
			ucPath.compare(L"CON") == 0 ||
			ucPath.compare(L"PRN") == 0 ||
			ucPath.compare(L"AUX") == 0 ||
			ucPath.compare(L"NUL") == 0 ||
			ucPath.compare(L"LPT1") == 0 ||
			ucPath.compare(L"LPT2") == 0 ||
			ucPath.compare(L"LPT3") == 0 ||
			ucPath.compare(L"LPT4") == 0 ||
			ucPath.compare(L"LPT5") == 0 ||
			ucPath.compare(L"LPT6") == 0 ||
			ucPath.compare(L"LPT7") == 0 ||
			ucPath.compare(L"LPT8") == 0 ||
			ucPath.compare(L"LPT9") == 0 ||
			ucPath.compare(L"COM1") == 0 ||
			ucPath.compare(L"COM2") == 0 ||
			ucPath.compare(L"COM3") == 0 ||
			ucPath.compare(L"COM4") == 0 ||
			ucPath.compare(L"COM5") == 0 ||
			ucPath.compare(L"COM6") == 0 ||
			ucPath.compare(L"COM7") == 0 ||
			ucPath.compare(L"COM8") == 0 ||
			ucPath.compare(L"COM9") == 0;
	}

	bool win32_isFile(const WString& path)
	{
		return !win32_isDirectory(path) && !win32_isDevice(path);
	}

	bool win32_createFile(const WString& path)
	{
		HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return true;
		}
		else if (GetLastError() == ERROR_FILE_EXISTS)
			return false;
		else
			win32_handleError(GetLastError(), path);

		return false;
	}


	bool win32_createDirectory(const WString& path)
	{
		if (win32_pathExists(path) && win32_isDirectory(path))
			return false;

		if (CreateDirectoryW(path.c_str(), 0) == FALSE)
			win32_handleError(GetLastError(), path);

		return true;
	}

	void win32_remove(const WString& path)
	{
		if (win32_isDirectory(path))
		{
			if (RemoveDirectoryW(path.c_str()) == 0)
				win32_handleError(GetLastError(), path);
		}
		else
		{
			if (DeleteFileW(path.c_str()) == 0)
				win32_handleError(GetLastError(), path);
		}
	}

	void win32_copy(const WString& from, const WString& to)
	{
		if (CopyFileW(from.c_str(), to.c_str(), FALSE) == FALSE)
			win32_handleError(GetLastError(), from);
	}


	void win32_rename(const WString& oldPath, const WString& newPath)
	{
		if (MoveFileW(oldPath.c_str(), newPath.c_str()) == 0)
			win32_handleError(GetLastError(), oldPath);
	}

	UINT64 win32_getFileSize(const WString& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA attrData;
		if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrData) == FALSE)
			win32_handleError(GetLastError(), path);

		LARGE_INTEGER li;
		li.LowPart = attrData.nFileSizeLow;
		li.HighPart = attrData.nFileSizeHigh;
		return (UINT64)li.QuadPart;
	}

	std::time_t win32_getLastModifiedTime(const WString& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad) == 0)
			win32_handleError(GetLastError(), path);

		ULARGE_INTEGER ull;
		ull.LowPart = fad.ftLastWriteTime.dwLowDateTime;
		ull.HighPart = fad.ftLastWriteTime.dwHighDateTime;

		return (std::time_t) ((ull.QuadPart / 10000000ULL) - 11644473600ULL);
	}

	DataStreamPtr FileSystem::openFile(const Path& fullPath, bool readOnly)
	{
		UINT64 fileSize = getFileSize(fullPath);

		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::in | std::ios::binary;
		std::shared_ptr<std::istream> baseStream = 0;
		std::shared_ptr<std::ifstream> roStream = 0;
		std::shared_ptr<std::fstream> rwStream = 0;

		if (!readOnly)
		{
			mode |= std::ios::out;
			rwStream = bs_shared_ptr<std::fstream, ScratchAlloc>();
			rwStream->open(fullPath.toWString().c_str(), mode);
			baseStream = rwStream;
		}
		else
		{
			roStream = bs_shared_ptr<std::ifstream, ScratchAlloc>();
			roStream->open(fullPath.toWString().c_str(), mode);
			baseStream = roStream;
		}

		// Should check ensure open succeeded, in case fail for some reason.
		if (baseStream->fail())
			BS_EXCEPT(FileNotFoundException, "Cannot open file: " + fullPath.toString());

		/// Construct return stream, tell it to delete on destroy
		FileDataStream* stream = 0;
		if (rwStream)
		{
			// use the writeable stream 
			stream = bs_new<FileDataStream, ScratchAlloc>(rwStream, (size_t)fileSize, true);
		}
		else
		{
			// read-only stream
			stream = bs_new<FileDataStream, ScratchAlloc>(roStream, (size_t)fileSize, true);
		}

		return bs_shared_ptr<FileDataStream, ScratchAlloc>(stream);
	}

	DataStreamPtr FileSystem::createAndOpenFile(const Path& fullPath)
	{
		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::out | std::ios::binary;
		std::shared_ptr<std::fstream> rwStream = bs_shared_ptr<std::fstream, ScratchAlloc>();
		rwStream->open(fullPath.toWString().c_str(), mode);

		// Should check ensure open succeeded, in case fail for some reason.
		if (rwStream->fail())
			BS_EXCEPT(FileNotFoundException, "Cannot open file: " + fullPath.toString());

		/// Construct return stream, tell it to delete on destroy
		return bs_shared_ptr<FileDataStream, ScratchAlloc>(rwStream, 0, true);
	}

	UINT64 FileSystem::getFileSize(const Path& fullPath)
	{
		return win32_getFileSize(fullPath.toWString());
	}

	void FileSystem::remove(const Path& fullPath, bool recursively)
	{
		WString fullPathStr = fullPath.toWString();

		if (!FileSystem::exists(fullPath))
			return;

		if (recursively)
		{
			Vector<Path> files;
			Vector<Path> directories;

			getChildren(fullPath, files, directories);

			for (auto& file : files)
				remove(file, false);

			for (auto& dir : directories)
				remove(dir, true);
		}

		win32_remove(fullPathStr);
	}

	void FileSystem::move(const Path& oldPath, const Path& newPath, bool overwriteExisting)
	{
		WString oldPathStr = oldPath.toWString();
		WString newPathStr = newPath.toWString();

		if (win32_pathExists(newPathStr))
		{
			if (overwriteExisting)
				win32_remove(newPathStr);
			else
			{
				BS_EXCEPT(InvalidStateException, "Move operation failed because another file already exists at the new path: \"" + toString(newPathStr) + "\"");
			}
		}

		win32_rename(oldPathStr, newPathStr);
	}

	bool FileSystem::exists(const Path& fullPath)
	{
		return win32_pathExists(fullPath.toWString());
	}

	bool FileSystem::isFile(const Path& fullPath)
	{
		WString pathStr = fullPath.toWString();

		return win32_pathExists(pathStr) && win32_isFile(pathStr);
	}

	bool FileSystem::isDirectory(const Path& fullPath)
	{
		WString pathStr = fullPath.toWString();

		return win32_pathExists(pathStr) && win32_isDirectory(pathStr);
	}

	void FileSystem::createDir(const Path& fullPath)
	{
		Path parentPath = fullPath;
		while (!exists(parentPath))
		{
			parentPath = parentPath.getParent();
		}

		for (UINT32 i = parentPath.getNumDirectories(); i < fullPath.getNumDirectories(); i++)
		{
			parentPath.append(fullPath[i]);
			win32_createDirectory(parentPath.toWString());
		}
	}

	void FileSystem::getChildren(const Path& dirPath, Vector<Path>& files, Vector<Path>& directories)
	{
		if (dirPath.isFile())
			return;

		WString findPath = dirPath.toWString();
		findPath.append(L"*");

		WIN32_FIND_DATAW findData;
		HANDLE fileHandle = FindFirstFileW(findPath.c_str(), &findData);

		bool lastFailed = false;
		WString tempName;
		do
		{
			if (lastFailed || fileHandle == INVALID_HANDLE_VALUE)
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					break;
				else
					win32_handleError(GetLastError(), findPath);
			}
			else
			{
				tempName = findData.cFileName;

				if (tempName != L"." && tempName != L"..")
				{
					Path fullPath = dirPath;
					if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
						directories.push_back(fullPath.append(tempName + L"/"));
					else
						files.push_back(fullPath.append(tempName));
				}
			}

			lastFailed = FindNextFileW(fileHandle, &findData) == FALSE;
		} while (true);

		FindClose(fileHandle);
	}

	std::time_t FileSystem::getLastModifiedTime(const Path& fullPath)
	{
		return win32_getLastModifiedTime(fullPath.toWString().c_str());
	}

	Path FileSystem::getWorkingDirectoryPath()
	{
		return Path(win32_getCurrentDirectory());
	}
}