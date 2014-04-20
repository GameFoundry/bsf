#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraRTTI : public RTTIType<Camera, Component, CameraRTTI>
	{
	private:

	public:
		CameraRTTI()
		{

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
			return cm_shared_ptr<Camera, PoolAlloc>(new (cm_alloc<Camera, PoolAlloc>()) Camera());
		}
	};
}