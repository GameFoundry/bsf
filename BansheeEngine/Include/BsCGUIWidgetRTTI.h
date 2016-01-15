#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsCGUIWidget.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT CGUIWidgetRTTI : public RTTIType <CGUIWidget, Component, CGUIWidgetRTTI>
	{
	private:

	public:
		CGUIWidgetRTTI()
		{ }

		virtual const String& getRTTIName() override
		{
			static String name = "CGUIWidget";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_CGUIWidget;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<CGUIWidget>();
		}
	};
}