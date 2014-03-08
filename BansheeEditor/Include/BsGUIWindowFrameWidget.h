#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIWidget.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class WindowFrameWidget : public BS::GUIWidget
	{
	public:
		WindowFrameWidget(const CM::HSceneObject& parent, CM::Viewport* target, CM::RenderWindow* ownerWindow, const BS::GUISkin& skin);
		virtual ~WindowFrameWidget();

	protected:
		static const CM::UINT32 RESIZE_BORDER_WIDTH;

		BS::GUIArea* mWindowFrameArea;
		CM::RenderWindow* mParentWindow;
		GUIWindowFrame* mWindowFrame;

		virtual void update();

		virtual bool _mouseEvent(BS::GUIElement* element, const BS::GUIMouseEvent& ev);
		virtual void ownerWindowFocusChanged();
		virtual void ownerTargetResized();

		void refreshNonClientAreas() const;
	};
}