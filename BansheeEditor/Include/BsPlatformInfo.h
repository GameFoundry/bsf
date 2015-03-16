#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	enum class PlatformType
	{
		Windows,
		Count // Keep at end
	};

	struct BS_ED_EXPORT PlatformInfo : public IReflectable
	{
		PlatformInfo();
		virtual ~PlatformInfo();

		PlatformType type;
		WString defines;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PlatformInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	struct BS_ED_EXPORT WinPlatformInfo : public PlatformInfo
	{
		WinPlatformInfo();

		bool is32bit;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class WinPlatformInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}