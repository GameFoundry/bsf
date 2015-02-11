#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIToggle.h"
#include "BsGUIToggleGroup.h"
#include "BsImageSprite.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class GUITabButton : public GUIToggle
	{
	public:
		static const String& getGUITypeName();

		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const HString& text, 
			const String& styleName = StringUtil::BLANK);
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const HString& text, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
	
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, 
			const String& styleName = StringUtil::BLANK);
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		UINT32 getIndex() const { return mIndex; }
		void _setDraggedState(bool active);

		Event<void(UINT32, const Vector2I&)> onDragged;
		Event<void(UINT32, const Vector2I&)> onDragEnd;
	protected:
		virtual bool _mouseEvent(const GUIMouseEvent& ev);

	private:
		UINT32 mIndex;
		Vector2I mDragStartPosition;
		bool mDraggedState;
		GUIButtonState mInactiveState;

		static const UINT32 DRAG_MIN_DISTANCE;

		GUITabButton(const String& styleName, const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}