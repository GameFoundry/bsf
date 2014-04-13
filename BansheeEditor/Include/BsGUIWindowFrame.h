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

		static GUIWindowFrame* create(const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIWindowFrame* create(const BS::GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();
		GUIWindowFrame(const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions);
	};
}