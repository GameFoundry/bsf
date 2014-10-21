#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCameraHandler.h"

namespace BansheeEngine
{
	class BS_EXPORT CameraHandlerRTTI : public RTTIType <CameraHandler, IReflectable, CameraHandlerRTTI>
	{
	private:

	public:
		CameraHandlerRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "CameraHandler";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_CameraHandler;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<CameraHandler>();
		}
	};
}