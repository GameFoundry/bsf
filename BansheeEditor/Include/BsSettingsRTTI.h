//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsSettings.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	class SettingsRTTI : public RTTIType <Settings, IReflectable, SettingsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mFloatProperties);
		BS_PLAIN_MEMBER(mIntProperties);
		BS_PLAIN_MEMBER(mBoolProperties);
		BS_PLAIN_MEMBER(mStringProperties);

	public:
		SettingsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mFloatProperties, 13);
			BS_ADD_PLAIN_FIELD(mIntProperties, 14);
			BS_ADD_PLAIN_FIELD(mBoolProperties, 15);
			BS_ADD_PLAIN_FIELD(mStringProperties, 16);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "Settings";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_Settings;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<Settings>();
		}
	};
}