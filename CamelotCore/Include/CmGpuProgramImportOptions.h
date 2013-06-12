#pragma once

#include "CmPrerequisites.h"
#include "CmImportOptions.h"
#include "CmGpuProgram.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuProgramImportOptions : public ImportOptions
	{
	public:
		GpuProgramImportOptions();

		void setEntryPoint(const String& entryPoint) { mEntryPoint = entryPoint; }
		void setLanguage(const String& language) { mLanguage = language; }
		void setProfile(GpuProgramProfile profile) { mProfile = profile; }
		void setType(GpuProgramType type) { mType = type; }
		void setIncludes(const Vector<HGpuProgInclude>::type& includes) { mIncludes = includes; }

		const String& getEntryPoint() const { return mEntryPoint; }
		const String& getLanguage() const { return mLanguage; }
		GpuProgramProfile getProfile() const { return mProfile; }
		GpuProgramType getType() const { return mType; }
		const Vector<HGpuProgInclude>::type& getIncludes() const { return mIncludes; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	private:
		String mEntryPoint;
		String mLanguage;
		GpuProgramProfile mProfile;
		GpuProgramType mType;
		Vector<HGpuProgInclude>::type mIncludes;
	};
}