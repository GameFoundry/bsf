#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"
#include "BsVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	 * @brief	Available platforms we can build for
	 */
	enum class PlatformType
	{
		Windows,
		Count // Keep at end
	};

	/**
	 * @brief	Contains per-platform information used primarily for build purposes.
	 */
	struct BS_ED_EXPORT PlatformInfo : public IReflectable
	{
		PlatformInfo();
		virtual ~PlatformInfo();

		PlatformType type; /**< Type of platform this object contains data for. */
		WString defines; /**< A set of semicolon separated defines to use when compiling scripts for this platform. */
		HPrefab mainScene; /**< Default scene that is loaded when the application is started. */
		bool fullscreen; /**< If true the application will be started in fullscreen using user's desktop resolution. */
		UINT32 windowedWidth; /**< Width of the window if not starting the application in fullscreen. */
		UINT32 windowedHeight; /**< Height of the window if not starting the application in fullscreen. */
		bool debug; /**< Determines should the scripts be output in debug mode (worse performance but better error reporting). */

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PlatformInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains Windows specific per-platform information used
	 *			primarily for build purposes.
	 */
	struct BS_ED_EXPORT WinPlatformInfo : public PlatformInfo
	{
		WinPlatformInfo();

		HTexture icon16;
		HTexture icon32;
		HTexture icon48;
		HTexture icon64;
		HTexture icon96;
		HTexture icon128;
		HTexture icon192;
		HTexture icon256;
		HTexture taskbarIcon;
		WString titlebarText;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class WinPlatformInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}