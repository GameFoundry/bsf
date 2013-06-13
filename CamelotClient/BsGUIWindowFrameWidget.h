#pragma once

#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class WindowFrameWidget : public BS::GUIWidget
	{
	public:
		WindowFrameWidget(const CM::HSceneObject& parent);
		virtual ~WindowFrameWidget();

		virtual void initialize(CM::Viewport* target, CM::RenderWindow* ownerWindow);

	protected:
		BS::GUIArea* mWindowFrameArea;
		BS::GUIWindowFrame* mWindowFrame;

		virtual void update();

		virtual bool _mouseEvent(BS::GUIElement* element, const BS::GUIMouseEvent& ev);
		virtual void ownerWindowFocusChanged();
	};
}