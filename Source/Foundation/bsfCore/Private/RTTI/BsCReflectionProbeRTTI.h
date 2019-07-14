//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Components/BsCReflectionProbe.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT CReflectionProbeRTTI : public RTTIType <CReflectionProbe, Component, CReflectionProbeRTTI>
	{
	private:
		SPtr<ReflectionProbe> getInternal(CReflectionProbe* obj) { return obj->mInternal; }
		void setInternal(CReflectionProbe* obj, SPtr<ReflectionProbe> val) { obj->mInternal = val; }

	public:
		CReflectionProbeRTTI()
		{
			addReflectablePtrField("mInternal", 0, &CReflectionProbeRTTI::getInternal, &CReflectionProbeRTTI::setInternal);
		}

		const String& getRTTIName() override
		{
			static String name = "CReflectionProbe";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CReflectionProbe;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return SceneObject::createEmptyComponent<CReflectionProbe>();
		}
	};

	/** @} */
	/** @endcond */
}
