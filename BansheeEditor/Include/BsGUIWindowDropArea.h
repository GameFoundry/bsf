#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class GUIWindowDropArea : public GUITexture
	{
	public:
		static const String& getGUITypeName();

		static GUIWindowDropArea* create(const String& styleName = StringUtil::BLANK);
		static GUIWindowDropArea* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setFocused(bool focused);

		Event<void()> onDraggedItemDropped;
	protected:
		~GUIWindowDropArea();
		GUIWindowDropArea(const String& styleName, const GUILayoutOptions& layoutOptions);

		virtual bool _mouseEvent(const GUIMouseEvent& ev);
	};
}