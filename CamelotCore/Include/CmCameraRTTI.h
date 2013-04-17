#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmComponent.h"

namespace CamelotFramework
{
	class CM_EXPORT CameraRTTI : public RTTIType<Camera, Component, CameraRTTI>
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
			return std::shared_ptr<Camera>(CM_NEW(Camera, PoolAlloc) Camera(), &MemAllocDeleter<Camera, PoolAlloc>::deleter);
		}
	};
}