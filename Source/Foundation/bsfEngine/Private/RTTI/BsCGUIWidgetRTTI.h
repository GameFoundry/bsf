//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "GUI/BsCGUIWidget.h"
#include "Private/RTTI/BsGameObjectRTTI.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT CGUIWidgetRTTI : public RTTIType <CGUIWidget, Component, CGUIWidgetRTTI>
	{
	private:

	public:
		CGUIWidgetRTTI() = default;

		const String& getRTTIName() override
		{
			static String name = "CGUIWidget";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_CGUIWidget;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CGUIWidget>();
		}
	};

	/** @} */
	/** @endcond */
}