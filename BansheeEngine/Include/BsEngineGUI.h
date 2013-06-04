#pragma once

#include "BsPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"

namespace BansheeEngine
{
	// TODO - This class is very Editor related, so move it there?
	class BS_EXPORT EngineGUI : public CamelotFramework::Module<EngineGUI>
	{
	public:
		EngineGUI();

		const GUISkin& getSkin() const { return mSkin; }

	private:
		GUISkin mSkin;

		static const CM::String DefaultFontPath;
		static const CM::UINT32 DefaultFontSize;

		static const CM::String WindowBackgroundTexture;

		static const CM::String WindowFrameNormal;
		static const CM::String WindowFrameFocused;

		static const CM::String WindowTitleBarBg;

		static const CM::String WindowCloseButtonNormal;
		static const CM::String WindowCloseButtonHover;

		static const CM::String WindowMinButtonNormal;
		static const CM::String WindowMinButtonHover;

		static const CM::String TabbedBarBtnNormal;
		static const CM::String TabbedBarBtnActive;

		static const CM::String ButtonNormalTex;
		static const CM::String ButtonHoverTex;
	};
}