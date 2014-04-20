#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include <boost/signals.hpp>

namespace BansheeEngine
{
	class GUIWindowDropArea : public GUITexture
	{
	public:
		static const String& getGUITypeName();

		static GUIWindowDropArea* create(const String& styleName = StringUtil::BLANK);
		static GUIWindowDropArea* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setFocused(bool focused);

		boost::signal<void()> onDraggedItemDropped;
	protected:
		~GUIWindowDropArea();
		GUIWindowDropArea(const String& styleName, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
	};
}