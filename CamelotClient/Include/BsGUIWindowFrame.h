#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include "BsImageSprite.h"

namespace BansheeEditor
{
	class GUIWindowFrame : public BS::GUITexture
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIWindowFrame* create(BS::GUIWidget& parent, const BS::GUIElementStyle* style = nullptr);
		static GUIWindowFrame* create(BS::GUIWidget& parent, const BS::GUILayoutOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();
		GUIWindowFrame(BS::GUIWidget& parent, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);
	};
}