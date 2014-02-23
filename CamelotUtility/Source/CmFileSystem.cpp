#include "CmFileSystem.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmException.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem3;

namespace CamelotFramework
{
	DataStreamPtr FileSystem::openFile(const WString& fullPath, bool readOnly)
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
			rwStream = cm_shared_ptr<std::fstream, ScratchAlloc>();
			rwStream->open(fullPath.c_str(), mode);
			baseStream = rwStream;
		}
		else
		{
			roStream = cm_shared_ptr<std::ifstream, ScratchAlloc>();
			roStream->open(fullPath.c_str(), mode);
			baseStream = roStream;
		}

		// Should check ensure open succeeded, in case fail for some reason.
		if (baseStream->fail())
			CM_EXCEPT(FileNotFoundException, "Cannot open file: " + toString(fullPath));

		/// Construct return stream, tell it to delete on destroy
		FileDataStream* stream = 0;
		if (rwStream)
		{
			// use the writeable stream 
			stream = cm_new<FileDataStream, ScratchAlloc>(toString(fullPath), rwStream, (size_t)fileSize, true);
		}
		else
		{
			// read-only stream
			stream = cm_new<FileDataStream, ScratchAlloc>(toString(fullPath), roStream, (size_t)fileSize, true);
		}
		return cm_shared_ptr<FileDataStream, ScratchAlloc>(stream);
	}

	DataStreamPtr FileSystem::createAndOpenFile(const WString& fullPath)
	{
		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::out | std::ios::binary;
		std::shared_ptr<std::fstream> rwStream = cm_shared_ptr<std::fstream, ScratchAlloc>();
		rwStream->open(fullPath.c_str(), mode);

		// Should check ensure open succeeded, in case fail for some reason.
		if (rwStream->fail())
			CM_EXCEPT(FileNotFoundException, "Cannot open file: " + toString(fullPath));

		/// Construct return stream, tell it to delete on destroy
		return cm_shared_ptr<FileDataStream, ScratchAlloc>(toString(fullPath), rwStream, 0, true);
	}

	UINT64 FileSystem::getFileSize(const WString& fullPath)
	{
		return file_size(fullPath.c_str());
	}

	void FileSystem::remove(const WString& fullPath, bool recursively)
	{
		if(recursively)
			boost::filesystem3::remove_all(fullPath.c_str());
		else
			boost::filesystem3::remove(fullPath.c_str());
	}

	void FileSystem::remove(const WPath& fullPath, bool recursively)
	{
		if(recursively)
			boost::filesystem3::remove_all(fullPath);
		else
			boost::filesystem3::remove(fullPath);
	}

	void FileSystem::move(const WPath& oldPath, const WPath& newPath, bool overwriteExisting)
	{
		if(FileSystem::isFile(newPath) || FileSystem::isDirectory(newPath))
		{
			if(overwriteExisting)
				FileSystem::remove(newPath, true);
			else
			{
				CM_EXCEPT(InvalidStateException, "Move operation failed because another file already exists at the new path: \"" + toString(newPath) + "\"");
			}
		}

		boost::filesystem3::rename(oldPath, newPath);
	}

	bool FileSystem::exists(const WString& fullPath)
	{
		return boost::filesystem3::exists(fullPath.c_str());
	}

	bool FileSystem::exists(const WPath& fullPath)
	{
		return boost::filesystem3::exists(fullPath);
	}

	bool FileSystem::isFile(const WString& fullPath)
	{
		if(boost::filesystem3::exists(fullPath.c_str()) && !is_directory(fullPath.c_str()))
			return true;

		return false;
	}

	bool FileSystem::isFile(const WPath& fullPath)
	{
		if(exists(fullPath) && !is_directory(fullPath))
			return true;

		return false;
	}

	bool FileSystem::isDirectory(const WString& fullPath)
	{
		if(boost::filesystem3::exists(fullPath.c_str()) && is_directory(fullPath.c_str()))
			return true;

		return false;
	}

	bool FileSystem::isDirectory(const WPath& fullPath)
	{
		if(boost::filesystem3::exists(fullPath) && is_directory(fullPath))
			return true;

		return false;
	}

	void FileSystem::createDir(const WString& fullPath)
	{
		createDir(WPath(fullPath.c_str()));
	}

	void FileSystem::createDir(const WPath& fullPath)
	{
		if(fullPath.empty())
			return;

		WPath parentPath = fullPath;
		auto pathEnd = fullPath.end();

		while(!exists(parentPath))
		{
			if(pathEnd == fullPath.begin())
				break;

			parentPath = parentPath.parent_path();
			pathEnd--;
		}

		for(; pathEnd != fullPath.end(); ++pathEnd)
		{
			create_directory(parentPath);

			parentPath /= *pathEnd;
		}

		create_directory(parentPath);
	}

	void FileSystem::getChildren(const WPath& dirPath, Vector<WPath>::type& files, Vector<WPath>::type& directories)
	{
		directory_iterator dirIter(dirPath.c_str());

		Vector<WString>::type foundFiles;
		while(dirIter != directory_iterator())
		{
			boost::filesystem3::path curPath = dirIter->path();

			if(is_regular_file(curPath))
				files.push_back(curPath);
			else if(is_directory(curPath))
				directories.push_back(curPath);

			dirIter++;
		}
	}

	std::time_t FileSystem::getLastModifiedTime(const WString& fullPath)
	{
		return last_write_time(fullPath.c_str());
	}

	std::time_t FileSystem::getLastModifiedTime(const WPath& fullPath)
	{
		return last_write_time(fullPath);
	}

	WString FileSystem::getWorkingDirectoryPath()
	{
		return current_path().wstring().c_str();
	}

	WString FileSystem::getParentDirectory(const WString& path)
	{
		boost::filesystem3::path p(path.c_str());
		
		if(!is_directory(p))
		{
			boost::filesystem3::path dir = p.parent_path();
			return dir.wstring().c_str();
		}

		return path;
	}
}