//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Platform/BsFolderMonitor.h"
#include "FileSystem/BsFileSystem.h"
#include "Error/BsException.h"
#include <corecrt_io.h>

namespace bs
{
	class WorkerFunc
	{
	public:
		WorkerFunc(FolderMonitor* owner);

		void operator()();

	private:
		FolderMonitor* mOwner;
	};

	struct FolderMonitor::FolderWatchInfo
	{
		FolderWatchInfo(const Path& folderToMonitor, int inHandle, bool monitorSubdirectories, UINT32 monitorFlags);
		~FolderWatchInfo();

		void startMonitor();
		void stopMonitor();

		Path mFolderToMonitor;
		int mDirHandle;
		bool mMonitorSubdirectories;
		UINT32 mMonitorFlags;
	};

	FolderMonitor::FolderWatchInfo::FolderWatchInfo(const Path& folderToMonitor, int inHandle, bool monitorSubdirectories, UINT32 monitorFlags)
		:mFolderToMonitor(folderToMonitor), mDirHandle(0), mMonitorSubdirectories(monitorSubdirectories), mMonitorFlags(monitorFlags)
	{ }

	FolderMonitor::FolderWatchInfo::~FolderWatchInfo()
	{
		stopMonitor();
	}

	void FolderMonitor::FolderWatchInfo::startMonitor()
	{
	}

	void FolderMonitor::FolderWatchInfo::stopMonitor()
	{
	}

	class FolderMonitor::FileNotifyInfo
	{
	};

	enum class FileActionType
	{
		Added,
		Removed,
		Modified,
		Renamed
	};

	struct FileAction
	{
		static FileAction* createAdded(const WString& fileName)
		{
			UINT8* bytes = (UINT8*)bs_alloc((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(WString::value_type)));

			FileAction* action = (FileAction*)bytes;
			bytes += sizeof(FileAction);

			action->oldName = nullptr;
			action->newName = (WString::value_type*)bytes;
			action->type = FileActionType::Added;

			memcpy(action->newName, fileName.data(), fileName.size() * sizeof(WString::value_type));
			action->newName[fileName.size()] = L'\0';
			action->lastSize = 0;
			action->checkForWriteStarted = false;

			return action;
		}

		static FileAction* createRemoved(const WString& fileName)
		{
			UINT8* bytes = (UINT8*)bs_alloc((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(WString::value_type)));

			FileAction* action = (FileAction*)bytes;
			bytes += sizeof(FileAction);

			action->oldName = nullptr;
			action->newName = (WString::value_type*)bytes;
			action->type = FileActionType::Removed;

			memcpy(action->newName, fileName.data(), fileName.size() * sizeof(WString::value_type));
			action->newName[fileName.size()] = L'\0';
			action->lastSize = 0;
			action->checkForWriteStarted = false;

			return action;
		}

		static FileAction* createModified(const WString& fileName)
		{
			UINT8* bytes = (UINT8*)bs_alloc((UINT32)(sizeof(FileAction) + (fileName.size() + 1) * sizeof(WString::value_type)));

			FileAction* action = (FileAction*)bytes;
			bytes += sizeof(FileAction);

			action->oldName = nullptr;
			action->newName = (WString::value_type*)bytes;
			action->type = FileActionType::Modified;

			memcpy(action->newName, fileName.data(), fileName.size() * sizeof(WString::value_type));
			action->newName[fileName.size()] = L'\0';
			action->lastSize = 0;
			action->checkForWriteStarted = false;

			return action;
		}

		static FileAction* createRenamed(const WString& oldFilename, const WString& newfileName)
		{
			UINT8* bytes = (UINT8*)bs_alloc((UINT32)(sizeof(FileAction) + 
				(oldFilename.size() + newfileName.size() + 2) * sizeof(WString::value_type)));

			FileAction* action = (FileAction*)bytes;
			bytes += sizeof(FileAction);

			action->oldName = (WString::value_type*)bytes;
			bytes += (oldFilename.size() + 1) * sizeof(WString::value_type);

			action->newName = (WString::value_type*)bytes;
			action->type = FileActionType::Modified;

			memcpy(action->oldName, oldFilename.data(), oldFilename.size() * sizeof(WString::value_type));
			action->oldName[oldFilename.size()] = L'\0';

			memcpy(action->newName, newfileName.data(), newfileName.size() * sizeof(WString::value_type));
			action->newName[newfileName.size()] = L'\0';
			action->lastSize = 0;
			action->checkForWriteStarted = false;

			return action;
		}

		static void destroy(FileAction* action)
		{
			bs_free(action);
		}

		WString::value_type* oldName;
		WString::value_type* newName;
		FileActionType type;

		UINT64 lastSize;
		bool checkForWriteStarted;
	};

	struct FolderMonitor::Pimpl
	{
		Vector<FolderWatchInfo*> mFoldersToWatch;

		Queue<FileAction*> mFileActions;
		List<FileAction*> mActiveFileActions;

		int inHandle;
		Mutex mMainMutex;
		Thread* mWorkerThread;
	};

	FolderMonitor::FolderMonitor()
	{
		mPimpl = bs_new<Pimpl>();
		mPimpl->mWorkerThread = nullptr;
		mPimpl->inHandle = 0;
	}

	FolderMonitor::~FolderMonitor()
	{
		stopMonitorAll();

		// No need for mutex since we know worker thread is shut down by now
		while (!mPimpl->mFileActions.empty())
		{
			FileAction* action = mPimpl->mFileActions.front();
			mPimpl->mFileActions.pop();

			FileAction::destroy(action);
		}

		bs_delete(mPimpl);
	}

	void FolderMonitor::startMonitor(const Path& folderPath, bool subdirectories, FolderChange changeFilter)
	{
		if(!FileSystem::isDirectory(folderPath))
		{
			LOGERR("Provided path \"" + folderPath.toString() + "\" is not a directory");
			return;
		}

		UINT32 filterFlags = 0;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::FileName) != 0)
			filterFlags |= IN_MOVE | IN_CREATE | IN_DELETE;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::DirName) != 0)
			filterFlags |= IN_MOVE | IN_CREATE | IN_DELETE;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::Attributes) != 0)
			filterFlags |= IN_ATTRIB;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::Size) != 0)
			filterFlags |= IN_ATTRIB | IN_MODIFY | IN_CREATE | IN_DELETE;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::LastWrite) != 0)
			filterFlags |= IN_ATTRIB | IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_MOVED_TO;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::LastAccess) != 0)
			filterFlags |= IN_ATTRIB | IN_ACCESS;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::Creation) != 0)
			filterFlags |= IN_CREATE;

		if((((UINT32)changeFilter) & (UINT32)bs::FolderChange::Security) != 0)
			filterFlags |= IN_ATTRIB;

		mPimpl->inHandle = inotify_init();

		mPimpl->mFoldersToWatch.push_back(bs_new<FolderWatchInfo>(folderPath, subdirectories, filterFlags));
		FolderWatchInfo* watchInfo = mPimpl->mFoldersToWatch.back();

		if(mPimpl->mWorkerThread == nullptr)
		{
			mPimpl->mWorkerThread = bs_new<Thread>(std::bind(&FolderMonitor::workerThreadMain, this));

			if(mPimpl->mWorkerThread == nullptr)
			{
				mPimpl->mFoldersToWatch.erase(mPimpl->mFoldersToWatch.end() - 1);
				bs_delete(watchInfo);
				BS_EXCEPT(InternalErrorException, "Failed to create a new worker thread for folder monitoring");
			}
		}

		if(mPimpl->mWorkerThread != nullptr)
		{
			watchInfo->startMonitor();
		}
		else
		{
			mPimpl->mFoldersToWatch.erase(mPimpl->mFoldersToWatch.end() - 1);
			bs_delete(watchInfo);
			BS_EXCEPT(InternalErrorException, "Failed to create a new worker thread for folder monitoring");
		}
	}

	void FolderMonitor::stopMonitor(const Path& folderPath)
	{
		auto findIter = std::find_if(mPimpl->mFoldersToWatch.begin(), mPimpl->mFoldersToWatch.end(), 
			[&](const FolderWatchInfo* x) { return x->mFolderToMonitor == folderPath; });

		if(findIter != mPimpl->mFoldersToWatch.end())
		{
			FolderWatchInfo* watchInfo = *findIter;

			watchInfo->stopMonitor();
			bs_delete(watchInfo);

			mPimpl->mFoldersToWatch.erase(findIter);
		}

		if(mPimpl->mFoldersToWatch.size() == 0)
			stopMonitorAll();
	}

	void FolderMonitor::stopMonitorAll()
	{
		for(auto& watchInfo : mPimpl->mFoldersToWatch)
		{
			watchInfo->stopMonitor();

			{
				// Note: Need this mutex to ensure worker thread is done with watchInfo.
				// Even though we wait for a condition variable from the worker thread in stopMonitor,
				// that doesn't mean the worker thread is done with the condition variable
				// (which is stored inside watchInfo)
				Lock lock(mPimpl->mMainMutex);
				bs_delete(watchInfo);
			}
		}

		mPimpl->mFoldersToWatch.clear();

		if(mPimpl->mWorkerThread != nullptr)
		{
			mPimpl->mWorkerThread->join();
			bs_delete(mPimpl->mWorkerThread);
			mPimpl->mWorkerThread = nullptr;
		}

		if(mPimpl->inHandle != 0)
		{
			close(mPimpl->inHandle);
			mPimpl->inHandle = 0;
		}
	}

	void FolderMonitor::workerThreadMain()
	{
	}

	void FolderMonitor::handleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
	{
	}

	void FolderMonitor::_update()
	{
		{
			Lock lock(mPimpl->mMainMutex);

			while (!mPimpl->mFileActions.empty())
			{
				FileAction* action = mPimpl->mFileActions.front();
				mPimpl->mFileActions.pop();

				mPimpl->mActiveFileActions.push_back(action);
			}
		}

		for (auto iter = mPimpl->mActiveFileActions.begin(); iter != mPimpl->mActiveFileActions.end();)
		{
			FileAction* action = *iter;
			
			// Reported file actions might still be in progress (i.e. something might still be writing to those files).
			// Sadly there doesn't seem to be a way to properly determine when those files are done being written, so instead
			// we check for at least a couple of frames if the file's size hasn't changed before reporting a file action.
			// This takes care of most of the issues and avoids reporting partially written files in almost all cases.
			if (FileSystem::exists(action->newName))
			{
				UINT64 size = FileSystem::getFileSize(action->newName);
				if (!action->checkForWriteStarted)
				{
					action->checkForWriteStarted = true;
					action->lastSize = size;

					++iter;
					continue;
				}
				else
				{
					if (action->lastSize != size)
					{
						action->lastSize = size;
						++iter;
						continue;
					}
				}
			}

			switch (action->type)
			{
			case FileActionType::Added:
				if (!onAdded.empty())
					onAdded(Path(action->newName));
				break;
			case FileActionType::Removed:
				if (!onRemoved.empty())
					onRemoved(Path(action->newName));
				break;
			case FileActionType::Modified:
				if (!onModified.empty())
					onModified(Path(action->newName));
				break;
			case FileActionType::Renamed:
				if (!onRenamed.empty())
					onRenamed(Path(action->oldName), Path(action->newName));
				break;
			}

			mPimpl->mActiveFileActions.erase(iter++);
			FileAction::destroy(action);
		}
	}
}
