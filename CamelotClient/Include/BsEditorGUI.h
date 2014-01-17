#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"

namespace BansheeEditor
{
	class EditorGUI : public CamelotFramework::Module<EditorGUI>
	{
	public:
		EditorGUI();

		const BS::GUISkin& getSkin() const { return mSkin; }

	private:
		BS::GUISkin mSkin;

		static const CM::String DefaultFolder;

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

		static const CM::String WindowMaxButtonNormal;
		static const CM::String WindowMaxButtonHover;

		static const CM::String TabbedBarBtnNormal;
		static const CM::String TabbedBarBtnActive;

		static const CM::String ButtonNormalTex;
		static const CM::String ButtonHoverTex;

		static const CM::String InputBoxNormalTex;
		static const CM::String InputBoxHoverTex;
		static const CM::String InputBoxFocusedTex;

		static const CM::String ScrollBarUpNormalTex;
		static const CM::String ScrollBarUpHoverTex;
		static const CM::String ScrollBarUpActiveTex;

		static const CM::String ScrollBarDownNormalTex;
		static const CM::String ScrollBarDownHoverTex;
		static const CM::String ScrollBarDownActiveTex;

		static const CM::String ScrollBarLeftNormalTex;
		static const CM::String ScrollBarLeftHoverTex;
		static const CM::String ScrollBarLeftActiveTex;

		static const CM::String ScrollBarRightNormalTex;
		static const CM::String ScrollBarRightHoverTex;
		static const CM::String ScrollBarRightActiveTex;

		static const CM::String ScrollBarHandleHorzNormalTex;
		static const CM::String ScrollBarHandleHorzHoverTex;
		static const CM::String ScrollBarHandleHorzActiveTex;

		static const CM::String ScrollBarHandleVertNormalTex;
		static const CM::String ScrollBarHandleVertHoverTex;
		static const CM::String ScrollBarHandleVertActiveTex;

		static const CM::String ScrollBarBgTex;

		static const CM::String DropDownBtnNormalTex;
		static const CM::String DropDownBtnHoverTex;

		static const CM::String DropDownBoxBgTex;
		static const CM::String DropDownBoxEntryNormalTex;
		static const CM::String DropDownBoxEntryHoverTex;

		static const CM::String DropDownBoxBtnUpNormalTex;
		static const CM::String DropDownBoxBtnUpHoverTex;

		static const CM::String DropDownBoxBtnDownNormalTex;
		static const CM::String DropDownBoxBtnDownHoverTex;

		static const CM::String DropDownBoxEntryExpNormalTex;
		static const CM::String DropDownBoxEntryExpHoverTex;

		static const CM::String DropDownSeparatorTex;

		static const CM::String DropDownBoxBtnUpArrowTex;
		static const CM::String DropDownBoxBtnDownArrowTex;

		static const CM::String MenuBarBgTex;

		static const CM::String MenuBarBtnNormalTex;
		static const CM::String MenuBarBtnHoverTex;

		static const CM::String MenuBarBansheeLogoTex;

		static const CM::String DockSliderNormalTex;

		static BS::HSpriteTexture getTexture(const CM::String& name);
	};
}