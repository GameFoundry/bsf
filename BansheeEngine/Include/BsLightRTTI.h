#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsLight.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT LightRTTI : public RTTIType <Light, Component, LightRTTI>
	{
	private:
		SPtr<LightInternal> getInternal(Light* obj) { return obj->mInternal; }
		void setInternal(Light* obj, SPtr<LightInternal> val) { obj->mInternal = val; }

	public:
		LightRTTI()
		{
			addReflectablePtrField("mInternal", 0, &LightRTTI::getInternal, &LightRTTI::setInternal);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "Light";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_Light;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<Light>();
		}
	};
}