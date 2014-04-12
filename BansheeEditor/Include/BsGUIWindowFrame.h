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

		static GUIWindowFrame* create(const BS::GUIElementStyle* style = nullptr);
		static GUIWindowFrame* create(const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();
		GUIWindowFrame(const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);
	};
}