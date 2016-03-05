//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsCCamera.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

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

		const String& getRTTIName() override
		{
			static String name = "CCamera";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CCamera;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CCamera>();
		}
	};

	/** @} */
	/** @endcond */
}