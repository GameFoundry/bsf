#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCamera.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraRTTI : public RTTIType<Camera, Component, CameraRTTI>
	{
	private:
		CameraHandlerPtr getInternal(Camera* obj) { return obj->mInternal; }
		void setInternal(Camera* obj, CameraHandlerPtr val) { obj->mInternal = val; }

	public:
		CameraRTTI()
		{
			addReflectablePtrField("mInternal", 0, &CameraRTTI::getInternal, &CameraRTTI::setInternal);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Camera";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Camera;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<Camera>();
		}
	};
}