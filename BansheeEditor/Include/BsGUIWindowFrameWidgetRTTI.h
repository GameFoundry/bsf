#pragma once

#include "BsEditorPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGUIWindowFrameWidget.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT WindowFrameWidgetRTTI : public RTTIType <WindowFrameWidget, CGUIWidget, WindowFrameWidgetRTTI>
	{
	private:

	public:
		WindowFrameWidgetRTTI()
		{ }

		virtual const String& getRTTIName() override
		{
			static String name = "WindowFrameWidget";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_WindowFrameWidget;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return GameObjectRTTI::createGameObject<WindowFrameWidget>();
		}
	};
}