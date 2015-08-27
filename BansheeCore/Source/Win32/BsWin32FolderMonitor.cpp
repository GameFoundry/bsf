#include "Win32/BsWin32FolderMonitor.h"
#include "BsFileSystem.h"
#include "BsException.h"

#include "BsDebug.h"

#include <windows.h>

namespace BansheeEngine
{
	enum class MonitorState
	{
		Inactive,
		Starting,
		Monitoring,
		Shutdown,
		Shutdown2
	};

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
		FolderWatchInfo(const Path& folderToMonitor, HANDLE dirHandle, bool monitorSubdirectories, DWORD monitorFlags);
		~FolderWatchInfo();

		void startMonitor(HANDLE compPortHandle);
		void stopMonitor(HANDLE compPortHandle);

		static const UINT32 READ_BUFFER_SIZE = 65536;

		Path mFolderToMonitor;
		HANDLE mDirHandle;
		OVERLAPPED mOverlapped;
		MonitorState mState;
		UINT8 mBuffer[READ_BUFFER_SIZE];
		DWORD mBufferSize;
		bool mMonitorSubdirectories;
		DWORD mMonitorFlags;
		DWORD mReadError;

		WString mCachedOldFileName; // Used during rename notifications as they are handled in two steps

		BS_MUTEX(mStatusMutex)
		BS_THREAD_SYNCHRONISER(mStartStopEvent)
	};

	FolderMonitor::FolderWatchInfo::FolderWatchInfo(const Path& folderToMonitor, HANDLE dirHandle, bool monitorSubdirectories, DWORD monitorFlags)
		:mFolderToMonitor(folderToMonitor), mDirHandle(dirHandle), mState(MonitorState::Inactive), mBufferSize(0),
		mMonitorSubdirectories(monitorSubdirectories), mMonitorFlags(monitorFlags), mReadError(0)
	{
		memset(&mOverlapped, 0, sizeof(mOverlapped));
	}

	FolderMonitor::FolderWatchInfo::~FolderWatchInfo()
	{
		assert(mState == MonitorState::Inactive);

		stopMonitor(0);
	}

	void FolderMonitor::FolderWatchInfo::startMonitor(HANDLE compPortHandle)
	{
		if(mState != MonitorState::Inactive)
			return; // Already monitoring

		{
			BS_LOCK_MUTEX_NAMED(mStatusMutex, lock);

			mState = MonitorState::Starting;
			PostQueuedCompletionStatus(compPortHandle, sizeof(this), (ULONG_PTR)this, &mOverlapped);

			while(mState != MonitorState::Monitoring)
				BS_THREAD_WAIT(mStartStopEvent, mStatusMutex, lock);
		}

		if(mReadError != ERROR_SUCCESS)
		{
			{
				BS_LOCK_MUTEX(mStatusMutex);
				mState = MonitorState::Inactive;
			}

			BS_EXCEPT(InternalErrorException, "Failed to start folder monitor on folder \"" + 
				mFolderToMonitor.toString() + "\" because ReadDirectoryChangesW failed.");
		}
	}

	void FolderMonitor::FolderWatchInfo::stopMonitor(HANDLE compPortHandle)
	{
		if(mState != MonitorState::Inactive)
		{
			BS_LOCK_MUTEX_NAMED(mStatusMutex, lock);

			mState = MonitorState::Shutdown;
			PostQueuedCompletionStatus(compPortHandle, sizeof(this), (ULONG_PTR)this, &mOverlapped);

			while(mState != MonitorState::Inactive)
				BS_THREAD_WAIT(mStartStopEvent, mStatusMutex, lock);
		}

		if(mDirHandle != INVALID_HANDLE_VALUE)
		{			
			CloseHandle(mDirHandle);
			mDirHandle = INVALID_HANDLE_VALUE;
		}
	}

	class FolderMonitor::FileNotifyInfo
	{
	public:
		FileNotifyInfo(UINT8* notifyBuffer, DWORD bufferSize)
		:mBuffer(notifyBuffer), mBufferSize(bufferSize)
		{
			mCurrentRecord = (PFILE_NOTIFY_INFORMATION)mBuffer;
		}

		bool getNext();
	
		DWORD	getAction() const;
		WString getFileName() const;
		WString getFileNameWithPath(const Path& rootPath) const;

	
	protected:
		UINT8* mBuffer;
		DWORD mBufferSize;
		PFILE_NOTIFY_INFORMATION mCurrentRecord;
	};

	bool FolderMonitor::FileNotifyInfo::getNext()
	{
		if(mCurrentRecord && mCurrentRecord->NextEntryOffset != 0)
		{
			PFILE_NOTIFY_INFORMATION oldRecord = mCurrentRecord;
			mCurrentRecord = (PFILE_NOTIFY_INFORMATION) ((UINT8*)mCurrentRecord + mCurrentRecord->NextEntryOffset);

			if((DWORD)((UINT8*)mCurrentRecord - mBuffer) > mBufferSize)
			{
				// Gone out of range, something bad happened
				assert(false);

				mCurrentRecord = oldRecord;
			}
					
			return (mCurrentRecord != oldRecord);
		}

		return false;
	}

	DWORD FolderMonitor::FileNotifyInfo::getAction() const
	{ 
		assert(mCurrentRecord != nullptr);

		if(mCurrentRecord)
			return mCurrentRecord->Action;

		return 0;
	}

	WString FolderMonitor::FileNotifyInfo::getFileName() const
	{
		if(mCurrentRecord)
		{
			wchar_t fileNameBuffer[32768 + 1] = {0};

			memcpy(fileNameBuffer, mCurrentRecord->FileName, 
					std::min(DWORD(32768 * sizeof(wchar_t)), mCurrentRecord->FileNameLength));
		
			return WString(fileNameBuffer);
		}

		return WString();
	}		

	WString FolderMonitor::FileNotifyInfo::getFileNameWithPath(const Path& rootPath) const
	{
		Path fullPath = rootPath;
		return fullPath.append(getFileName()).toWString();
	}

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

			return action;
		}

		static void destroy(FileAction* action)
		{
			bs_free(action);
		}

		WString::value_type* oldName;
		WString::value_type* newName;
		FileActionType type;
	};

	struct FolderMonitor::Pimpl
	{
		Vector<FolderWatchInfo*> mFoldersToWatch;
		HANDLE mCompPortHandle;

		Queue<FileAction*> mFileActions;
		Queue<FileAction*> mActiveFileActions;

		BS_MUTEX(mMainMutex);
		BS_THREAD_TYPE* mWorkerThread;
	};

	FolderMonitor::FolderMonitor()
	{
		mPimpl = bs_new<Pimpl>();
		mPimpl->mWorkerThread = nullptr;
		mPimpl->mCompPortHandle = nullptr;
	}

	FolderMonitor::~FolderMonitor()
	{
		stopMonitorAll();

		// No need for mutex since we know worker thread is shut down by now
		while(!mPimpl->mFileActions.empty())
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
			BS_EXCEPT(InvalidParametersException, "Provided path \"" + folderPath.toString() + "\" is not a directory");
		}

		WString extendedFolderPath = L"\\\\?\\" + folderPath.toWString(Path::PathType::Windows);
		HANDLE dirHandle = CreateFileW(extendedFolderPath.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

		if(dirHandle == INVALID_HANDLE_VALUE)
		{
			BS_EXCEPT(InternalErrorException, "Failed to open folder \"" + folderPath.toString() + "\" for monitoring. Error code: " + toString((UINT64)GetLastError()));
		}

		DWORD filterFlags = 0;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::FileName) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_FILE_NAME;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::DirName) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_DIR_NAME;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::Attributes) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_ATTRIBUTES;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::Size) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_SIZE;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::LastWrite) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_LAST_WRITE;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::LastAccess) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_LAST_ACCESS;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::Creation) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_CREATION;

		if((((UINT32)changeFilter) & (UINT32)BansheeEngine::FolderChange::Security) != 0)
			filterFlags |= FILE_NOTIFY_CHANGE_SECURITY;

		mPimpl->mFoldersToWatch.push_back(bs_new<FolderWatchInfo>(folderPath, dirHandle, subdirectories, filterFlags));
		FolderWatchInfo* watchInfo = mPimpl->mFoldersToWatch.back();

		mPimpl->mCompPortHandle = CreateIoCompletionPort(dirHandle, mPimpl->mCompPortHandle, (ULONG_PTR)watchInfo, 0);

		if(mPimpl->mCompPortHandle == nullptr)
		{
			mPimpl->mFoldersToWatch.erase(mPimpl->mFoldersToWatch.end() - 1);
			bs_delete(watchInfo);
			BS_EXCEPT(InternalErrorException, "Failed to open completition port for folder monitoring. Error code: " + toString((UINT64)GetLastError()));
		}

		if(mPimpl->mWorkerThread == nullptr)
		{
			BS_THREAD_CREATE(t, (std::bind(&FolderMonitor::workerThreadMain, this)));
			mPimpl->mWorkerThread = t;

			if(mPimpl->mWorkerThread == nullptr)
			{
				mPimpl->mFoldersToWatch.erase(mPimpl->mFoldersToWatch.end() - 1);
				bs_delete(watchInfo);
				BS_EXCEPT(InternalErrorException, "Failed to create a new worker thread for folder monitoring");
			}
		}

		if(mPimpl->mWorkerThread != nullptr)
		{
			try
			{
				watchInfo->startMonitor(mPimpl->mCompPortHandle);
			}
			catch (Exception* e)
			{
				mPimpl->mFoldersToWatch.erase(mPimpl->mFoldersToWatch.end() - 1);
				bs_delete(watchInfo);
				throw(e);
			}
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

			watchInfo->stopMonitor(mPimpl->mCompPortHandle);
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
			watchInfo->stopMonitor(mPimpl->mCompPortHandle);

			{
				// Note: Need this mutex to ensure worker thread is done with watchInfo.
				// Even though we wait for a condition variable from the worker thread in stopMonitor,
				// that doesn't mean the worker thread is done with the condition variable
				// (which is stored inside watchInfo)
				BS_LOCK_MUTEX(mPimpl->mMainMutex); 
				bs_delete(watchInfo);
			}
		}

		mPimpl->mFoldersToWatch.clear();

		if(mPimpl->mWorkerThread != nullptr)
		{
			PostQueuedCompletionStatus(mPimpl->mCompPortHandle, 0, 0, nullptr);

			mPimpl->mWorkerThread->join();
			BS_THREAD_DESTROY(mPimpl->mWorkerThread);
			mPimpl->mWorkerThread = nullptr;
		}

		if(mPimpl->mCompPortHandle != nullptr)
		{
			CloseHandle(mPimpl->mCompPortHandle);
			mPimpl->mCompPortHandle = nullptr;
		}
	}

	void FolderMonitor::workerThreadMain()
	{
		FolderWatchInfo* watchInfo = nullptr;

		do 
		{
			DWORD numBytes;
			LPOVERLAPPED overlapped;

			if(!GetQueuedCompletionStatus(mPimpl->mCompPortHandle, &numBytes, (PULONG_PTR) &watchInfo, &overlapped, INFINITE))
			{
				assert(false);
				// TODO: Folder handle was lost most likely. Not sure how to deal with that. Shutdown watch on this folder and cleanup?
			}

			if(watchInfo != nullptr)
			{
				MonitorState state;

				{
					BS_LOCK_MUTEX(watchInfo->mStatusMutex);
					state = watchInfo->mState;
				}

				switch(state)
				{
				case MonitorState::Starting:
					if(!ReadDirectoryChangesW(watchInfo->mDirHandle, watchInfo->mBuffer, FolderWatchInfo::READ_BUFFER_SIZE,
						watchInfo->mMonitorSubdirectories, watchInfo->mMonitorFlags, &watchInfo->mBufferSize, &watchInfo->mOverlapped, nullptr))
					{
						assert(false); // TODO - Possibly the buffer was too small?
						watchInfo->mReadError = GetLastError();
					}
					else
					{
						watchInfo->mReadError = ERROR_SUCCESS;

						{
							BS_LOCK_MUTEX(watchInfo->mStatusMutex);
							watchInfo->mState = MonitorState::Monitoring;
						}
					}

					BS_THREAD_NOTIFY_ONE(watchInfo->mStartStopEvent);

					break;
				case MonitorState::Monitoring:
					{
						FileNotifyInfo info(watchInfo->mBuffer, FolderWatchInfo::READ_BUFFER_SIZE);
						handleNotifications(info, *watchInfo);

						if(!ReadDirectoryChangesW(watchInfo->mDirHandle, watchInfo->mBuffer, FolderWatchInfo::READ_BUFFER_SIZE,
							watchInfo->mMonitorSubdirectories, watchInfo->mMonitorFlags, &watchInfo->mBufferSize, &watchInfo->mOverlapped, nullptr))
						{
							assert(false); // TODO: Failed during normal operation, possibly the buffer was too small. Shutdown watch on this folder and cleanup?
							watchInfo->mReadError = GetLastError();
						}
						else
						{
							watchInfo->mReadError = ERROR_SUCCESS;
						}
					}
					break;
				case MonitorState::Shutdown:
					if(watchInfo->mDirHandle != INVALID_HANDLE_VALUE)
					{
						CloseHandle(watchInfo->mDirHandle);
						watchInfo->mDirHandle = INVALID_HANDLE_VALUE;

						{
							BS_LOCK_MUTEX(watchInfo->mStatusMutex);
							watchInfo->mState = MonitorState::Shutdown2;
						}
					}
					else
					{
						{
							BS_LOCK_MUTEX(watchInfo->mStatusMutex);
							watchInfo->mState = MonitorState::Inactive;
						}

						{
							BS_LOCK_MUTEX(mPimpl->mMainMutex); // Ensures that we don't delete "watchInfo" before this thread is done with mStartStopEvent
							BS_THREAD_NOTIFY_ONE(watchInfo->mStartStopEvent);
						}
					}

					break;
				case MonitorState::Shutdown2:
					if(watchInfo->mDirHandle != INVALID_HANDLE_VALUE)
					{
						// Handle is still open? Try again.
						CloseHandle(watchInfo->mDirHandle);
						watchInfo->mDirHandle = INVALID_HANDLE_VALUE;
					}
					else
					{
						{
							BS_LOCK_MUTEX(watchInfo->mStatusMutex);
							watchInfo->mState = MonitorState::Inactive;
						}

						{
							BS_LOCK_MUTEX(mPimpl->mMainMutex); // Ensures that we don't delete "watchInfo" before this thread is done with mStartStopEvent
							BS_THREAD_NOTIFY_ONE(watchInfo->mStartStopEvent);
						}
					}

					break;
				}
			}

		} while (watchInfo != nullptr);
	}

	void FolderMonitor::handleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo)
	{
		Vector<FileAction*> mActions;

		do
		{
			switch(notifyInfo.getAction())
			{
			case FILE_ACTION_ADDED:
				{
					WString fileName = notifyInfo.getFileNameWithPath(watchInfo.mFolderToMonitor); 
					mActions.push_back(FileAction::createAdded(fileName));
				}
				break;
			case FILE_ACTION_REMOVED:
				{
					WString fileName = notifyInfo.getFileNameWithPath(watchInfo.mFolderToMonitor); 
					mActions.push_back(FileAction::createRemoved(fileName));
				}
				break;
			case FILE_ACTION_MODIFIED:
				{
					WString fileName = notifyInfo.getFileNameWithPath(watchInfo.mFolderToMonitor); 
					mActions.push_back(FileAction::createModified(fileName));
				}
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				{
					WString fileName = notifyInfo.getFileNameWithPath(watchInfo.mFolderToMonitor);
					watchInfo.mCachedOldFileName = fileName;
				}
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				{
					WString fileName = notifyInfo.getFileNameWithPath(watchInfo.mFolderToMonitor);
					mActions.push_back(FileAction::createRenamed(watchInfo.mCachedOldFileName, fileName));
				}
				break;

			}
    
		} while(notifyInfo.getNext());

		{
			BS_LOCK_MUTEX(mPimpl->mMainMutex);

			for(auto& action : mActions)
				mPimpl->mFileActions.push(action);
		}
	}

	void FolderMonitor::_update()
	{
		{
			BS_LOCK_MUTEX(mPimpl->mMainMutex);

			mPimpl->mActiveFileActions.swap(mPimpl->mFileActions);
		}

		while(!mPimpl->mActiveFileActions.empty())
		{
			FileAction* action = mPimpl->mActiveFileActions.front();
			mPimpl->mActiveFileActions.pop();

			switch(action->type)
			{
			case FileActionType::Added:
				if(!onAdded.empty())
					onAdded(Path(action->newName));
				break;
			case FileActionType::Removed:
				if(!onRemoved.empty())
					onRemoved(Path(action->newName));
				break;
			case FileActionType::Modified:
				if(!onModified.empty())
					onModified(Path(action->newName));
				break;
			case FileActionType::Renamed:
				if(!onRenamed.empty())
					onRenamed(Path(action->oldName), Path(action->newName));
				break;
			}

			FileAction::destroy(action);
		}
	}
}