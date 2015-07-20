#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGUIWidget.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWidgetRTTI : public RTTIType <GUIWidget, Component, GUIWidgetRTTI>
	{
	private:

	public:
		GUIWidgetRTTI()
		{ }

		virtual const String& getRTTIName() override
		{
			static String name = "GUIWidget";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_GUIWidget;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<GUIWidget>();
		}
	};
}