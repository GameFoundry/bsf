#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	enum class FolderChange
	{
		FileName = 0x0001,
		DirName = 0x0002,
		Attributes = 0x0004,
		Size = 0x0008,
		LastWrite = 0x0010,
		LastAccess = 0x0020,
		Creation = 0x0040,
		Security = 0x0080
	};

	class CM_EXPORT FolderMonitor
	{
		struct Pimpl;
		class FileNotifyInfo;
		struct FolderWatchInfo;
	public:
		FolderMonitor();
		~FolderMonitor();

		void startMonitor(const WString& folderPath, bool subdirectories, FolderChange changeFilter);
		void stopMonitor(const WString& folderPath);
		void stopMonitorAll();

		/**
		 * @brief	Callbacks will only get fired after update is called().
		 * 			
		 * @note	Internal method.
		 */
		void update();

		boost::signal<void(const WString&)> onModified;
		boost::signal<void(const WString&)> onAdded;
		boost::signal<void(const WString&)> onRemoved;
		boost::signal<void(const WString&, const WString&)> onRenamed;

	private:
		Pimpl* mPimpl;

		void workerThreadMain();
		void handleNotifications(FileNotifyInfo& notifyInfo, FolderWatchInfo& watchInfo);
	};
}