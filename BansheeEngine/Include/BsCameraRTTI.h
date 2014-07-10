//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
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