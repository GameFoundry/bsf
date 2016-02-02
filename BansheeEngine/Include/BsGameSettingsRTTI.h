//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameSettings.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT GameSettingsRTTI : public RTTIType <GameSettings, IReflectable, GameSettingsRTTI>
	{
	private:
		String& getMainSceneUUID(GameSettings* obj) { return obj->mainSceneUUID; }
		void setMainSceneUUID(GameSettings* obj, String& val) { obj->mainSceneUUID = val; }

		bool& getFullscreen(GameSettings* obj) { return obj->fullscreen; }
		void setFullscreen(GameSettings* obj, bool& val) { obj->fullscreen = val; }

		bool& getUseDesktopResolution(GameSettings* obj) { return obj->useDesktopResolution; }
		void setUseDesktopResolution(GameSettings* obj, bool& val) { obj->useDesktopResolution = val; }

		UINT32& getResolutionWidth(GameSettings* obj) { return obj->resolutionWidth; }
		void setResolutionWidth(GameSettings* obj, UINT32& val) { obj->resolutionWidth = val; }

		UINT32& getResolutionHeight(GameSettings* obj) { return obj->resolutionHeight; }
		void setResolutionHeight(GameSettings* obj, UINT32& val) { obj->resolutionHeight = val; }

		WString& getTitlebarText(GameSettings* obj) { return obj->titleBarText; }
		void setTitlebarText(GameSettings* obj, WString& val) { obj->titleBarText = val; }

	public:
		GameSettingsRTTI()
		{
			addPlainField("mainSceneUUID", 0, &GameSettingsRTTI::getMainSceneUUID, &GameSettingsRTTI::setMainSceneUUID);
			addPlainField("fullscreen", 1, &GameSettingsRTTI::getFullscreen, &GameSettingsRTTI::setFullscreen);
			addPlainField("useDesktopResolution", 2, &GameSettingsRTTI::getUseDesktopResolution, &GameSettingsRTTI::setUseDesktopResolution);
			addPlainField("resolutionWidth", 3, &GameSettingsRTTI::getResolutionWidth, &GameSettingsRTTI::setResolutionWidth);
			addPlainField("resolutionHeight", 4, &GameSettingsRTTI::getResolutionHeight, &GameSettingsRTTI::setResolutionHeight);
			addPlainField("titleBarText", 5, &GameSettingsRTTI::getTitlebarText, &GameSettingsRTTI::setTitlebarText);
		}

		const String& getRTTIName() override
		{
			static String name = "GameSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GameSettings;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<GameSettings>();
		}
	};

	/** @} */
	/** @endcond */
}