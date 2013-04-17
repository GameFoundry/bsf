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

		virtual UINT32 getRTTIId()
		{
			return TID_Camera;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return std::shared_ptr<Camera>(CM_NEW(Camera, CM::PoolAlloc) Camera(), &CM::MemAllocDeleter<Camera, CM::PoolAlloc>::deleter);
		}
	};
}