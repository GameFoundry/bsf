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
			return bs_shared_ptr<Camera, PoolAlloc>(new (bs_alloc<Camera, PoolAlloc>()) Camera());
		}
	};
}