#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"
#include "BsModule.h"
#include "BsPlatformInfo.h"

namespace BansheeEngine
{
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
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_ED_EXPORT BuildManager : public Module<BuildManager>
	{
	public:
		BuildManager();

		const Vector<PlatformType>& getAvailablePlatforms() const;
		PlatformType getActivePlatform() const;
		void setActivePlatform(PlatformType type);

		SPtr<PlatformInfo> getActivePlatformInfo() const;
		SPtr<PlatformInfo> getPlatformInfo(PlatformType type) const;

		Vector<WString> getFrameworkAssemblies(PlatformType type) const;
		Path getMainExecutable(PlatformType type) const;
		WString getDefines(PlatformType type) const;

		void save(const Path& outFile);
		void load(const Path& inFile);

	private:
		SPtr<BuildData> mBuildData;
	};
}