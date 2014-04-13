#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include <boost/signals.hpp>

namespace BansheeEditor
{
	class GUIWindowDropArea : public BS::GUITexture
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIWindowDropArea* create(const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIWindowDropArea* create(const BS::GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		void setFocused(bool focused);

		boost::signal<void()> onDraggedItemDropped;
	protected:
		~GUIWindowDropArea();
		GUIWindowDropArea(const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
	};
}