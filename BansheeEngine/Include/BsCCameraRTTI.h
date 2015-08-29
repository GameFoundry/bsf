#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCCamera.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT CCameraRTTI : public RTTIType<CCamera, Component, CCameraRTTI>
	{
	private:
		CameraPtr getInternal(CCamera* obj) { return obj->mInternal; }
		void setInternal(CCamera* obj, CameraPtr val) { obj->mInternal = val; }

	public:
		CCameraRTTI()
		{
			addReflectablePtrField("mInternal", 0, &CCameraRTTI::getInternal, &CCameraRTTI::setInternal);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "CCamera";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_CCamera;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CCamera>();
		}
	};
}