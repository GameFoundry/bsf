#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraRTTI : public CM::RTTIType<Camera, CM::Component, CameraRTTI>
	{
	private:

	public:
		CameraRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "Camera";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_Camera;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<Camera, CM::PoolAlloc>(new (CM::cm_alloc<Camera, CM::PoolAlloc>()) Camera());
		}
	};
}