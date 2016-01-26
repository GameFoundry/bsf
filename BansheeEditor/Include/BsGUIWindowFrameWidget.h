#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIWidget.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class WindowFrameWidget : public GUIWidget
	{
	public:
		WindowFrameWidget(const HSceneObject& parent, Viewport* target, RenderWindow* ownerWindow, const GUISkin& skin);
		virtual ~WindowFrameWidget();

	protected:
		static const UINT32 RESIZE_BORDER_WIDTH;

		GUIArea* mWindowFrameArea;
		RenderWindow* mParentWindow;
		GUIWindowFrame* mWindowFrame;

		virtual void update();

		virtual bool _mouseEvent(GUIElement* element, const GUIMouseEvent& ev);
		virtual void ownerWindowFocusChanged();
		virtual void ownerTargetResized();

		void refreshNonClientAreas() const;
	};
}