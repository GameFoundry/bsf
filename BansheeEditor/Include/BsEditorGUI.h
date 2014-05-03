#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"
#include "CmPath.h"

namespace BansheeEngine
{
	class EditorGUI : public BansheeEngine::Module<EditorGUI>
	{
	public:
		EditorGUI();

		const GUISkin& getSkin() const { return mSkin; }

	private:
		GUISkin mSkin;

		static const Path DefaultFolder;

		static const WString DefaultFontPath;
		static const UINT32 DefaultFontSize;

		static const WString WindowBackgroundTexture;

		static const WString WindowFrameNormal;
		static const WString WindowFrameFocused;

		static const WString WindowTitleBarBg;

		static const WString WindowCloseButtonNormal;
		static const WString WindowCloseButtonHover;

		static const WString WindowMinButtonNormal;
		static const WString WindowMinButtonHover;

		static const WString WindowMaxButtonNormal;
		static const WString WindowMaxButtonHover;

		static const WString TabbedBarBtnNormal;
		static const WString TabbedBarBtnActive;

		static const WString ButtonNormalTex;
		static const WString ButtonHoverTex;
		static const WString ButtonActiveTex;

		static const WString ToggleNormalTex;
		static const WString ToggleHoverTex;
		static const WString ToggleActiveTex;
		static const WString ToggleNormalOnTex;
		static const WString ToggleHoverOnTex;
		static const WString ToggleActiveOnTex;

		static const WString ObjectDropBtnNormalTex;
		static const WString ObjectDropBtnNormalOnTex;
		static const WString ObjectClearBtnNormalTex;
		static const WString ObjectClearBtnHoverTex;
		static const WString ObjectClearBtnActiveTex;

		static const WString FoldoutOpenNormalTex;
		static const WString FoldoutOpenHoverTex;
		static const WString FoldoutClosedNormalTex;
		static const WString FoldoutClosedHoverTex;
		static const WString FoldoutBackgroundTex;

		static const WString InputBoxNormalTex;
		static const WString InputBoxHoverTex;
		static const WString InputBoxFocusedTex;

		static const WString ScrollBarUpNormalTex;
		static const WString ScrollBarUpHoverTex;
		static const WString ScrollBarUpActiveTex;

		static const WString ScrollBarDownNormalTex;
		static const WString ScrollBarDownHoverTex;
		static const WString ScrollBarDownActiveTex;

		static const WString ScrollBarLeftNormalTex;
		static const WString ScrollBarLeftHoverTex;
		static const WString ScrollBarLeftActiveTex;

		static const WString ScrollBarRightNormalTex;
		static const WString ScrollBarRightHoverTex;
		static const WString ScrollBarRightActiveTex;

		static const WString ScrollBarHandleHorzNormalTex;
		static const WString ScrollBarHandleHorzHoverTex;
		static const WString ScrollBarHandleHorzActiveTex;

		static const WString ScrollBarHandleVertNormalTex;
		static const WString ScrollBarHandleVertHoverTex;
		static const WString ScrollBarHandleVertActiveTex;

		static const WString ScrollBarBgTex;

		static const WString DropDownBtnNormalTex;
		static const WString DropDownBtnHoverTex;

		static const WString DropDownBoxBgTex;
		static const WString DropDownBoxEntryNormalTex;
		static const WString DropDownBoxEntryHoverTex;

		static const WString DropDownBoxBtnUpNormalTex;
		static const WString DropDownBoxBtnUpHoverTex;

		static const WString DropDownBoxBtnDownNormalTex;
		static const WString DropDownBoxBtnDownHoverTex;

		static const WString DropDownBoxEntryExpNormalTex;
		static const WString DropDownBoxEntryExpHoverTex;

		static const WString DropDownSeparatorTex;

		static const WString DropDownBoxBtnUpArrowTex;
		static const WString DropDownBoxBtnDownArrowTex;

		static const WString MenuBarBgTex;

		static const WString MenuBarBtnNormalTex;
		static const WString MenuBarBtnHoverTex;

		static const WString MenuBarBansheeLogoTex;

		static const WString DockSliderNormalTex;

		static const WString TreeViewExpandButtonOffNormal;
		static const WString TreeViewExpandButtonOffHover;
		static const WString TreeViewExpandButtonOnNormal;
		static const WString TreeViewExpandButtonOnHover;

		static const WString TreeViewSelectionBackground;
		static const WString TreeViewEditBox;

		static const WString TreeViewElementHighlight;
		static const WString TreeViewElementSepHighlight;

		static HSpriteTexture getTexture(const WString& name);
	};
}