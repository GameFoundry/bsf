#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	class GUIWindowFrame : public GUITexture
	{
	public:
		static const String& getGUITypeName();

		static GUIWindowFrame* create(const String& styleName = StringUtil::BLANK);
		static GUIWindowFrame* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();
		GUIWindowFrame(const String& styleName, const GUILayoutOptions& layoutOptions);
	};
}