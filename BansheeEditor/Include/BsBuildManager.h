#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"
#include "BsModule.h"
#include "BsPlatformInfo.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains build information for a specific platform to be
	 *			used by the build manager.
	 */
	class BS_ED_EXPORT BuildData : public IReflectable
	{
	public:
		BuildData();

		PlatformType activePlatform;
		Vector<SPtr<PlatformInfo>> platformData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class BuildDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Types of various folders used by the build manager.
	 */
	enum class BuildFolder
	{
		SourceRoot, /**< Absolute path to the root folder where all the prebuilt binaries and data exist. */
		DestinationRoot, /**< Absolute path to the root folder for a build for a specific platform. */
		NativeBinaries, /**< Folder where native binaries are stored. Relative to root. */
		BansheeAssemblies, /**< Folder where Banshee specific assemblies are stored. Relative to root. */
		Data /**< Folder where builtin data is stored. Relative to root. */
	};

	/**
	 * @brief	Handles building of the game executable and related files.
	 */
	class BS_ED_EXPORT BuildManager : public Module<BuildManager>
	{
	public:
		BuildManager();

		/**
		 * @brief	Returns a list of available platforms the executable can be built for.
		 */
		const Vector<PlatformType>& getAvailablePlatforms() const;

		/**
		 * @brief	Returns the currently active platform.
		 */
		PlatformType getActivePlatform() const;

		/**
		 * @brief	Changes the active build platform. Might cause asset reimport.
		 */
		void setActivePlatform(PlatformType type);

		/**
		 * @brief	Gets stored build setting for the active platform.
		 */
		SPtr<PlatformInfo> getActivePlatformInfo() const;

		/**
		 * @brief	Gets stored build setting for a specific platform.
		 */
		SPtr<PlatformInfo> getPlatformInfo(PlatformType type) const;

		/**
		 * @brief	Returns a list of file names (without extension) of all .NET assemblies required for a specific platform.
		 */
		Vector<WString> getFrameworkAssemblies(PlatformType type) const;

		/**
		 * @brief	Returns a list names of all native binaries required for a specific platform.
		 */
		Vector<Path> getNativeBinaries(PlatformType type) const;

		/**
		 * @brief	Returns a path to a specific folder used in the build process. See entries of
		 *			BuildFolder enum for explanations of individual folder types.
		 */
		Path getBuildFolder(BuildFolder folder, PlatformType platform) const;

		/**
		 * @brief	Returns the absolute path of the pre-built executable for the specified platform.
		 */
		Path getMainExecutable(PlatformType type) const;

		/**
		 * @brief	Returns a list of script defines for a specific platform.
		 */
		WString getDefines(PlatformType type) const;

		/**
		 * @brief	Stores build settings for all platforms in the specified file.
		 */
		void save(const Path& outFile);

		/**
		 * @brief	Loads a previously stored list of build settings.
		 */
		void load(const Path& inFile);

		/**
		 * @brief	Clears currently active build settings.
		 */
		void clear();

	private:
		static const WString BUILD_FOLDER_NAME;

		SPtr<BuildData> mBuildData;
	};
}