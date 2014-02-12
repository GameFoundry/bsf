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

		static const CM::WString DefaultFolder;

		static const CM::WString DefaultFontPath;
		static const CM::UINT32 DefaultFontSize;

		static const CM::WString WindowBackgroundTexture;

		static const CM::WString WindowFrameNormal;
		static const CM::WString WindowFrameFocused;

		static const CM::WString WindowTitleBarBg;

		static const CM::WString WindowCloseButtonNormal;
		static const CM::WString WindowCloseButtonHover;

		static const CM::WString WindowMinButtonNormal;
		static const CM::WString WindowMinButtonHover;

		static const CM::WString WindowMaxButtonNormal;
		static const CM::WString WindowMaxButtonHover;

		static const CM::WString TabbedBarBtnNormal;
		static const CM::WString TabbedBarBtnActive;

		static const CM::WString ButtonNormalTex;
		static const CM::WString ButtonHoverTex;

		static const CM::WString InputBoxNormalTex;
		static const CM::WString InputBoxHoverTex;
		static const CM::WString InputBoxFocusedTex;

		static const CM::WString ScrollBarUpNormalTex;
		static const CM::WString ScrollBarUpHoverTex;
		static const CM::WString ScrollBarUpActiveTex;

		static const CM::WString ScrollBarDownNormalTex;
		static const CM::WString ScrollBarDownHoverTex;
		static const CM::WString ScrollBarDownActiveTex;

		static const CM::WString ScrollBarLeftNormalTex;
		static const CM::WString ScrollBarLeftHoverTex;
		static const CM::WString ScrollBarLeftActiveTex;

		static const CM::WString ScrollBarRightNormalTex;
		static const CM::WString ScrollBarRightHoverTex;
		static const CM::WString ScrollBarRightActiveTex;

		static const CM::WString ScrollBarHandleHorzNormalTex;
		static const CM::WString ScrollBarHandleHorzHoverTex;
		static const CM::WString ScrollBarHandleHorzActiveTex;

		static const CM::WString ScrollBarHandleVertNormalTex;
		static const CM::WString ScrollBarHandleVertHoverTex;
		static const CM::WString ScrollBarHandleVertActiveTex;

		static const CM::WString ScrollBarBgTex;

		static const CM::WString DropDownBtnNormalTex;
		static const CM::WString DropDownBtnHoverTex;

		static const CM::WString DropDownBoxBgTex;
		static const CM::WString DropDownBoxEntryNormalTex;
		static const CM::WString DropDownBoxEntryHoverTex;

		static const CM::WString DropDownBoxBtnUpNormalTex;
		static const CM::WString DropDownBoxBtnUpHoverTex;

		static const CM::WString DropDownBoxBtnDownNormalTex;
		static const CM::WString DropDownBoxBtnDownHoverTex;

		static const CM::WString DropDownBoxEntryExpNormalTex;
		static const CM::WString DropDownBoxEntryExpHoverTex;

		static const CM::WString DropDownSeparatorTex;

		static const CM::WString DropDownBoxBtnUpArrowTex;
		static const CM::WString DropDownBoxBtnDownArrowTex;

		static const CM::WString MenuBarBgTex;

		static const CM::WString MenuBarBtnNormalTex;
		static const CM::WString MenuBarBtnHoverTex;

		static const CM::WString MenuBarBansheeLogoTex;

		static const CM::WString DockSliderNormalTex;

		static const CM::WString TreeViewExpandButtonOffNormal;
		static const CM::WString TreeViewExpandButtonOffHover;
		static const CM::WString TreeViewExpandButtonOnNormal;
		static const CM::WString TreeViewExpandButtonOnHover;

		static const CM::WString TreeViewSelectionBackground;
		static const CM::WString TreeViewEditBox;

		static const CM::WString TreeViewElementHighlight;
		static const CM::WString TreeViewElementSepHighlight;

		static BS::HSpriteTexture getTexture(const CM::WString& name);
	};
}