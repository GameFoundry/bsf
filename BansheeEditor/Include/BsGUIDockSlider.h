#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsDockManager.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class GUIDockSlider : public GUIButtonBase
	{
	public:
		static const String& getGUITypeName();

		static GUIDockSlider* create(bool horizontal, const String& styleName = StringUtil::BLANK);
		static GUIDockSlider* create(bool horizontal, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		Event<void(const Vector2I&)> onDragged;
	protected:
		virtual bool mouseEvent(const GUIMouseEvent& ev);

		bool _hasCustomCursor(const Vector2I position, CursorType& type) const;
	private:
		Vector2I mLastDragPosition;
		bool mHorizontal;
		bool mIsCursorSet;
		bool mDragInProgress;

		GUIDockSlider(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);
	};
}