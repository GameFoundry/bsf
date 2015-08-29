#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCLight.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT CLightRTTI : public RTTIType <CLight, Component, CLightRTTI>
	{
	private:
		SPtr<Light> getInternal(CLight* obj) { return obj->mInternal; }
		void setInternal(CLight* obj, SPtr<Light> val) { obj->mInternal = val; }

	public:
		CLightRTTI()
		{
			addReflectablePtrField("mInternal", 0, &CLightRTTI::getInternal, &CLightRTTI::setInternal);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "CLight";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_CLight;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CLight>();
		}
	};
}