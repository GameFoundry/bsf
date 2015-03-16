#include "BsBuildManager.h"
#include "BsBuildDataRTTI.h"
#include "BsFileSerializer.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	BuildData::BuildData()
		:activePlatform(PlatformType::Windows)
	{
		platformData.resize((UINT32)PlatformType::Count);
	}

	RTTITypeBase* BuildData::getRTTIStatic()
	{
		return BuildDataRTTI::instance();
	}

	RTTITypeBase* BuildData::getRTTI() const
	{
		return BuildData::getRTTIStatic();
	}

	BuildManager::BuildManager()
	{
		mBuildData = bs_shared_ptr<BuildData>();
	}

	const Vector<PlatformType>& BuildManager::getAvailablePlatforms() const
	{
		static const Vector<PlatformType> PLATFORMS = { PlatformType::Windows };

		return PLATFORMS;
	}

	PlatformType BuildManager::getActivePlatform() const
	{
		return mBuildData->activePlatform;
	}

	void BuildManager::setActivePlatform(PlatformType type)
	{
		if ((UINT32)type < (UINT32)PlatformType::Count)
			mBuildData->activePlatform = type;
	}

	SPtr<PlatformInfo> BuildManager::getActivePlatformInfo() const
	{
		return mBuildData->platformData[(UINT32)mBuildData->activePlatform];
	}

	SPtr<PlatformInfo> BuildManager::getPlatformInfo(PlatformType type) const
	{
		if ((UINT32)type < (UINT32)mBuildData->platformData.size())
			return mBuildData->platformData[(UINT32)type];

		return nullptr;
	}

	Vector<WString> BuildManager::getFrameworkAssemblies(PlatformType type) const
	{
		switch (type)
		{
		case PlatformType::Windows:
		default:
			return { L"System", L"System.Core" };
		}
	}

	Path BuildManager::getMainExecutable(PlatformType type) const
	{
		switch (type)
		{
		case PlatformType::Windows:
		{
			SPtr<WinPlatformInfo> winPlatformInfo = std::static_pointer_cast<WinPlatformInfo>(getPlatformInfo(type));

			if (winPlatformInfo->is32bit)
				return "Prebuilt\\Win32\\Game.exe";
			else
				return "Prebuilt\\Win64\\Game.exe";
		}
		}

		return Path::BLANK;
	}

	WString BuildManager::getDefines(PlatformType type) const
	{
		return getPlatformInfo(type)->defines;
	}

	void BuildManager::save(const Path& outFile)
	{
		FileEncoder fe(outFile);
		fe.encode(mBuildData.get());
	}

	void BuildManager::load(const Path& inFile)
	{
		if (!FileSystem::exists(inFile))
			return;

		FileDecoder fd(inFile);
		mBuildData = std::static_pointer_cast<BuildData>(fd.decode());
	}
}