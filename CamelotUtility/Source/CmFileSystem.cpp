#include "CmFileSystem.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmException.h"

#include <boost/filesystem.hpp>

#include <sys/types.h>
#include <sys/stat.h>

#if CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE || \
	CM_PLATFORM == CM_PLATFORM_SYMBIAN || CM_PLATFORM == CM_PLATFORM_IPHONE
#   include <sys/param.h>
#   define MAX_PATH MAXPATHLEN
#endif

#if CM_PLATFORM == CM_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#  include <direct.h>
#  include <io.h>
#endif

namespace CamelotEngine
{
	DataStreamPtr FileSystem::open(const String& fullPath, bool readOnly)
	{
		// Use filesystem to determine size 
		// (quicker than streaming to the end and back)
		struct stat tagStat;
		int ret = stat(fullPath.c_str(), &tagStat);
		assert(ret == 0 && "Problem getting file size" );
		(void)ret;  // Silence warning

		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::in | std::ios::binary;
		std::istream* baseStream = 0;
		std::ifstream* roStream = 0;
		std::fstream* rwStream = 0;

		if (!readOnly)
		{
			mode |= std::ios::out;
			rwStream = new std::fstream();
			rwStream->open(fullPath.c_str(), mode);
			baseStream = rwStream;
		}
		else
		{
			roStream = new std::ifstream();
			roStream->open(fullPath.c_str(), mode);
			baseStream = roStream;
		}

		// Should check ensure open succeeded, in case fail for some reason.
		if (baseStream->fail())
		{
			if(roStream != nullptr)
				delete roStream;

			if(rwStream != nullptr)
				delete rwStream;

			CM_EXCEPT(FileNotFoundException, "Cannot open file: " + fullPath);
		}

		/// Construct return stream, tell it to delete on destroy
		FileDataStream* stream = 0;
		if (rwStream)
		{
			// use the writeable stream 
			stream = new FileDataStream(fullPath, rwStream, (size_t)tagStat.st_size, true);
		}
		else
		{
			// read-only stream
			stream = new FileDataStream(fullPath, roStream, (size_t)tagStat.st_size, true);
		}
		return DataStreamPtr(stream);
	}

	DataStreamPtr FileSystem::create(const String& fullPath)
	{
		// Always open in binary mode
		// Also, always include reading
		std::ios::openmode mode = std::ios::out | std::ios::binary;
		std::fstream* rwStream = new std::fstream();
		rwStream->open(fullPath.c_str(), mode);

		// Should check ensure open succeeded, in case fail for some reason.
		if (rwStream->fail())
		{
			if(rwStream != nullptr)
				delete rwStream;

			CM_EXCEPT(FileNotFoundException, "Cannot open file: " + fullPath);
		}

		/// Construct return stream, tell it to delete on destroy
		FileDataStream* stream = new FileDataStream(fullPath, rwStream, 0, true);
		return DataStreamPtr(stream);
	}

	void FileSystem::remove(const String& fullPath)
	{
		::remove(fullPath.c_str());
	}

	bool FileSystem::fileExists(const String& fullPath)
	{
		if(boost::filesystem::exists(fullPath) && !boost::filesystem::is_directory(fullPath))
			return true;

		return false;
	}

	bool FileSystem::dirExists(const String& fullPath)
	{
		if(boost::filesystem::exists(fullPath) && boost::filesystem::is_directory(fullPath))
			return true;

		return false;
	}

	void FileSystem::createDir(const String& fullPath)
	{
		boost::filesystem::create_directory(fullPath);
	}

	void FileSystem::deleteDir(const String& fullPath)
	{
		boost::filesystem::remove_all(fullPath);
	}

	vector<String>::type FileSystem::getFiles(const String& dirPath)
	{
		boost::filesystem::directory_iterator dirIter(dirPath);

		vector<String>::type foundFiles;
		
		while(dirIter != boost::filesystem::directory_iterator())
		{
			if(boost::filesystem::is_regular_file(dirIter->path()))
				foundFiles.push_back(dirIter->path().string());

			dirIter++;
		}

		return foundFiles;
	}

	String FileSystem::getCurrentPath()
	{
		return boost::filesystem::current_path().string();
	}

	bool FileSystem::isValidFileName(const String& name)
	{
		return boost::filesystem::portable_file_name(name);
	}

	String FileSystem::getDirectoryPath(const String& path)
	{
		boost::filesystem::path p(path);

		if(!is_directory(p))
		{
			boost::filesystem::path dir = p.parent_path();
			return dir.string();
		}

		return path;
	}
}