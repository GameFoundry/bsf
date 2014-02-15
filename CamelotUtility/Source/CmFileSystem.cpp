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
			remove_all(fullPath.c_str());
		else
			boost::filesystem3::remove(fullPath.c_str());
	}

	bool FileSystem::isFile(const WString& fullPath)
	{
		if(exists(fullPath.c_str()) && !is_directory(fullPath.c_str()))
			return true;

		return false;
	}

	bool FileSystem::isDirectory(const WString& fullPath)
	{
		if(exists(fullPath.c_str()) && is_directory(fullPath.c_str()))
			return true;

		return false;
	}

	void FileSystem::createDir(const WString& fullPath)
	{
		create_directory(fullPath.c_str());
	}

	Vector<WString>::type FileSystem::getFiles(const WString& dirPath)
	{
		directory_iterator dirIter(dirPath.c_str());

		Vector<WString>::type foundFiles;
		while(dirIter != directory_iterator())
		{
			if(is_regular_file(dirIter->path()))
				foundFiles.push_back(dirIter->path().wstring().c_str());

			dirIter++;
		}

		return foundFiles;
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