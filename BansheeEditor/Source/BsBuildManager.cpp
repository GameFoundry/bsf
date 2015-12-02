#include "BsBuildManager.h"
#include "BsBuildDataRTTI.h"
#include "BsFileSerializer.h"
#include "BsFileSystem.h"
#include "BsEditorApplication.h"

namespace BansheeEngine
{
	BuildData::BuildData()
		:activePlatform(PlatformType::Windows)
	{
		platformData.resize((UINT32)PlatformType::Count);
		platformData[0] = bs_shared_ptr_new<WinPlatformInfo>();
	}

	RTTITypeBase* BuildData::getRTTIStatic()
	{
		return BuildDataRTTI::instance();
	}

	RTTITypeBase* BuildData::getRTTI() const
	{
		return BuildData::getRTTIStatic();
	}

	const WString BuildManager::BUILD_FOLDER_NAME = L"Builds\\";

	BuildManager::BuildManager()
	{
		mBuildData = bs_shared_ptr_new<BuildData>();
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

	Vector<Path> BuildManager::getNativeBinaries(PlatformType type) const
	{
		Vector<Path> libs = { L"BansheeEngine", L"BansheeCore", L"BansheeUtility",
			L"BansheeD3D11RenderAPI", L"BansheeGLRenderAPI", L"BansheeMono", L"BansheeOISInput",
			L"RenderBeast", L"SBansheeEngine", L"BansheeOIS", L"mono-2.0", L"nvtt" };

		switch (type)
		{
		case PlatformType::Windows:
		{
			for (auto& lib : libs)
				lib.setExtension(".dll");
		}
		}

		return libs;
	}

	Path BuildManager::getBuildFolder(BuildFolder folder, PlatformType platform) const
	{
		Path sourceRoot = APP_ROOT;
		sourceRoot.makeAbsolute(FileSystem::getWorkingDirectoryPath());

		switch (folder)
		{
		case BuildFolder::SourceRoot:
			return sourceRoot;
		case BuildFolder::DestinationRoot:
		{
			switch (platform)
			{
			case PlatformType::Windows:
				return gEditorApplication().getProjectPath() + BUILD_FOLDER_NAME + L"Windows";
			}

			return gEditorApplication().getProjectPath() + BUILD_FOLDER_NAME;
		}
		case BuildFolder::NativeBinaries:
		{
			Path binariesPath = FileSystem::getWorkingDirectoryPath();

			return binariesPath.makeRelative(sourceRoot);
		}
		case BuildFolder::BansheeAssemblies:
		{
			Path dataPath = ASSEMBLY_PATH;
			dataPath.makeAbsolute(FileSystem::getWorkingDirectoryPath());

			return dataPath.makeRelative(sourceRoot);
		}
		case BuildFolder::Data:
		{
			Path dataPath = ENGINE_DATA_PATH;
			dataPath.makeAbsolute(FileSystem::getWorkingDirectoryPath());

			return dataPath.makeRelative(sourceRoot);
		}
		}

		return Path::BLANK;
	}

	Path BuildManager::getMainExecutable(PlatformType type) const
	{
		switch (type)
		{
		case PlatformType::Windows:
		{
			Path output = RUNTIME_DATA_PATH + "Binaries\\Win64\\Game.exe";
			output.makeAbsolute(FileSystem::getWorkingDirectoryPath());

			return output;
		}
		}

		return Path::BLANK;
	}

	WString BuildManager::getDefines(PlatformType type) const
	{
		return getPlatformInfo(type)->defines;
	}

	void BuildManager::clear()
	{
		mBuildData = nullptr;
	}

	void BuildManager::save(const Path& outFile)
	{
		FileEncoder fe(outFile);
		fe.encode(mBuildData.get());
	}

	void BuildManager::load(const Path& inFile)
	{
		if (FileSystem::exists(inFile))
		{
			FileDecoder fd(inFile);
			mBuildData = std::static_pointer_cast<BuildData>(fd.decode());
		}

		if (mBuildData == nullptr)
			mBuildData = bs_shared_ptr_new<BuildData>();
	}
}