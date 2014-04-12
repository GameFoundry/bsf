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

		static GUIWindowDropArea* create(const BS::GUIElementStyle* style = nullptr);
		static GUIWindowDropArea* create(const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		void setFocused(bool focused);

		boost::signal<void()> onDraggedItemDropped;
	protected:
		~GUIWindowDropArea();
		GUIWindowDropArea(const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
	};
}