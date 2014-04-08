#include "BsEditorGUI.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUIToggle.h"
#include "BsGUIColor.h"
#include "BsTextSprite.h"
#include "BsSpriteTexture.h"
#include "BsGUITreeViewEditBox.h"

#include "CmFont.h"
#include "CmFontImportOptions.h"
#include "CmImporter.h"
#include "CmRTTIType.h"
#include "CmFileSystem.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const WString EditorGUI::DefaultFontPath = L"arial.ttf";
	const UINT32 EditorGUI::DefaultFontSize = 10;

	const WString EditorGUI::DefaultFolder = L"..\\..\\..\\..\\Data\\Editor\\Skin\\";

	const WString EditorGUI::WindowBackgroundTexture = L"WindowBgTile.psd";

	const WString EditorGUI::ButtonNormalTex = L"ButtonNormal.psd";
	const WString EditorGUI::ButtonHoverTex = L"ButtonHover.psd";

	const WString EditorGUI::ToggleNormalTex = L"ToggleNormal.psd";
	const WString EditorGUI::ToggleHoverTex = L"ToggleHover.psd";
	const WString EditorGUI::ToggleActiveTex = L"ToggleActive.psd";
	const WString EditorGUI::ToggleNormalOnTex = L"ToggleOnNormal.psd";
	const WString EditorGUI::ToggleHoverOnTex = L"ToggleOnHover.psd";
	const WString EditorGUI::ToggleActiveOnTex = L"ToggleOnActive.psd";

	const WString EditorGUI::FoldoutOpenNormalTex = L"FoldoutOpenNormal.psd";
	const WString EditorGUI::FoldoutOpenHoverTex = L"FoldoutOpenHover.psd";
	const WString EditorGUI::FoldoutClosedNormalTex = L"FoldoutClosedNormal.psd";
	const WString EditorGUI::FoldoutClosedHoverTex = L"FoldoutClosedHover.psd";
	const WString EditorGUI::FoldoutBackgroundTex = L"FoldoutBackground.psd";

	const WString EditorGUI::WindowFrameNormal = L"WindowFrameNormal.psd";
	const WString EditorGUI::WindowFrameFocused = L"WindowFrameFocused.psd";

	const WString EditorGUI::WindowTitleBarBg = L"WindowTitleBarBg.psd";

	const WString EditorGUI::WindowCloseButtonNormal = L"WindowCloseBtnNormal.psd";
	const WString EditorGUI::WindowCloseButtonHover = L"WindowCloseBtnHover.psd";

	const WString EditorGUI::WindowMinButtonNormal = L"WindowMaxBtnNormal.psd";
	const WString EditorGUI::WindowMinButtonHover = L"WindowMaxBtnHover.psd";

	const WString EditorGUI::WindowMaxButtonNormal = L"WindowMinBtnNormal.psd";
	const WString EditorGUI::WindowMaxButtonHover = L"WindowMinBtnHover.psd";

	const WString EditorGUI::TabbedBarBtnNormal = L"TabbedButtonNormal.psd";
	const WString EditorGUI::TabbedBarBtnActive = L"TabbedButtonActive.psd";

	const WString EditorGUI::InputBoxNormalTex = L"InputBoxNormal.psd";
	const WString EditorGUI::InputBoxHoverTex = L"InputBoxHover.psd";
	const WString EditorGUI::InputBoxFocusedTex = L"InputBoxFocused.psd";

	const WString EditorGUI::ScrollBarUpNormalTex = L"ScrollBarUpNormal.psd";
	const WString EditorGUI::ScrollBarUpHoverTex = L"ScrollBarUpHover.psd";
	const WString EditorGUI::ScrollBarUpActiveTex = L"ScrollBarUpActive.psd";

	const WString EditorGUI::ScrollBarDownNormalTex = L"ScrollBarDownNormal.psd";
	const WString EditorGUI::ScrollBarDownHoverTex = L"ScrollBarDownHover.psd";
	const WString EditorGUI::ScrollBarDownActiveTex = L"ScrollBarDownActive.psd";

	const WString EditorGUI::ScrollBarLeftNormalTex = L"ScrollBarLeftNormal.psd";
	const WString EditorGUI::ScrollBarLeftHoverTex = L"ScrollBarLeftHover.psd";
	const WString EditorGUI::ScrollBarLeftActiveTex = L"ScrollBarLeftActive.psd";

	const WString EditorGUI::ScrollBarRightNormalTex = L"ScrollBarRightNormal.psd";
	const WString EditorGUI::ScrollBarRightHoverTex = L"ScrollBarRightHover.psd";
	const WString EditorGUI::ScrollBarRightActiveTex = L"ScrollBarRightActive.psd";

	const WString EditorGUI::ScrollBarHandleHorzNormalTex = L"ScrollBarHorzHandleNormal.psd";
	const WString EditorGUI::ScrollBarHandleHorzHoverTex = L"ScrollBarHorzHandleHover.psd";
	const WString EditorGUI::ScrollBarHandleHorzActiveTex = L"ScrollBarHorzHandleActive.psd";

	const WString EditorGUI::ScrollBarHandleVertNormalTex = L"ScrollBarVertHandleNormal.psd";
	const WString EditorGUI::ScrollBarHandleVertHoverTex = L"ScrollBarVertHandleHover.psd";
	const WString EditorGUI::ScrollBarHandleVertActiveTex = L"ScrollBarVertHandleActive.psd";

	const WString EditorGUI::DropDownBtnNormalTex = L"DropDownNormal.psd";
	const WString EditorGUI::DropDownBtnHoverTex = L"DropDownHover.psd";

	const WString EditorGUI::DropDownBoxBgTex = L"DropDownBoxBg.psd";
	const WString EditorGUI::DropDownBoxEntryNormalTex = L"DropDownButtonNormal.psd";
	const WString EditorGUI::DropDownBoxEntryHoverTex = L"DropDownButtonHover.psd";

	const WString EditorGUI::DropDownBoxBtnUpNormalTex = L"DropDownBoxBtnUpNormal.psd";
	const WString EditorGUI::DropDownBoxBtnUpHoverTex = L"DropDownBoxBtnUpHover.psd";

	const WString EditorGUI::DropDownBoxBtnDownNormalTex = L"DropDownBoxBtnDownNormal.psd";
	const WString EditorGUI::DropDownBoxBtnDownHoverTex = L"DropDownBoxBtnDownHover.psd";

	const WString EditorGUI::DropDownBoxEntryExpNormalTex = L"DropDownExpNormal.psd";
	const WString EditorGUI::DropDownBoxEntryExpHoverTex = L"DropDownExpHover.psd";

	const WString EditorGUI::DropDownSeparatorTex = L"DropDownSeparator.psd";

	const WString EditorGUI::DropDownBoxBtnUpArrowTex = L"DropDownBoxBtnUpArrow.psd";
	const WString EditorGUI::DropDownBoxBtnDownArrowTex = L"DropDownBoxBtnDownArrow.psd";

	const WString EditorGUI::ScrollBarBgTex = L"ScrollBarBg.psd";

	const WString EditorGUI::MenuBarBgTex = L"MenuBarBg.psd";

	const WString EditorGUI::MenuBarBtnNormalTex = L"MenuBarButtonNormal.psd";
	const WString EditorGUI::MenuBarBtnHoverTex = L"MenuBarButtonHover.psd";

	const WString EditorGUI::MenuBarBansheeLogoTex = L"MenuBarBansheeLogo.psd";

	const WString EditorGUI::DockSliderNormalTex = L"DockSliderBtn.psd";

	const WString EditorGUI::TreeViewExpandButtonOffNormal = L"TreeViewExpandButtonOffNormal.psd";
	const WString EditorGUI::TreeViewExpandButtonOffHover = L"TreeViewExpandButtonOffHover.psd";
	const WString EditorGUI::TreeViewExpandButtonOnNormal = L"TreeViewExpandButtonOnNormal.psd";
	const WString EditorGUI::TreeViewExpandButtonOnHover = L"TreeViewExpandButtonOnHover.psd";

	const WString EditorGUI::TreeViewSelectionBackground = L"TreeViewSelectionBackground.psd";
	const WString EditorGUI::TreeViewEditBox = L"TreeViewEditBox.psd";
	const WString EditorGUI::TreeViewElementHighlight = L"TreeViewElementHighlight.psd";
	const WString EditorGUI::TreeViewElementSepHighlight = L"TreeViewElementSepHighlight.psd";

	EditorGUI::EditorGUI()
	{
		// TODO - Normally I want to load this from some file

		// Label
		// TODO - Instead of importing font every time, try to save a resource and then just load it?
		HFont font;

		{
			WString fontPath = DefaultFolder + DefaultFontPath;
			ImportOptionsPtr fontImportOptions = Importer::instance().createImportOptions(fontPath);
			if(rtti_is_of_type<FontImportOptions>(fontImportOptions))
			{
				FontImportOptions* importOptions = static_cast<FontImportOptions*>(fontImportOptions.get());

				Vector<CamelotFramework::UINT32>::type fontSizes;
				fontSizes.push_back(DefaultFontSize);
				importOptions->setFontSizes(fontSizes);
				importOptions->setAntialiasing(false);
			}

			font = Importer::instance().import(fontPath, fontImportOptions);
		}

		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = DefaultFontSize;
		labelStyle.fixedWidth = false;
		labelStyle.fixedHeight = true;
		labelStyle.height = 16;
		labelStyle.minWidth = 10;

		mSkin.setStyle(GUILabel::getGUITypeName(), labelStyle);

		// Window frame
		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = getTexture(WindowFrameNormal);
		windowFrameStyle.focused.texture = getTexture(WindowFrameFocused);
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		mSkin.setStyle("WindowFrame", windowFrameStyle);

		// Button
		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = getTexture(ButtonNormalTex);
		buttonStyle.hover.texture = getTexture(ButtonHoverTex);
		buttonStyle.active.texture = buttonStyle.hover.texture;
		buttonStyle.border.left = 5;
		buttonStyle.border.right = 5;
		buttonStyle.border.top = 5;
		buttonStyle.border.bottom = 5;
		buttonStyle.margins.left = 4;
		buttonStyle.margins.right = 4;
		buttonStyle.margins.top = 4;
		buttonStyle.margins.bottom = 4;
		buttonStyle.contentOffset.left = 2;
		buttonStyle.contentOffset.right = 2;
		buttonStyle.fixedHeight = true;
		buttonStyle.height = 21;
		buttonStyle.minWidth = 50;
		buttonStyle.font = font;
		buttonStyle.fontSize = DefaultFontSize;
		buttonStyle.textHorzAlign = THA_Center;
		buttonStyle.textVertAlign = TVA_Center;

		mSkin.setStyle(GUIButton::getGUITypeName(), buttonStyle);

		// Toggle
		GUIElementStyle toggleStyle;
		toggleStyle.normal.texture = getTexture(ToggleNormalTex);
		toggleStyle.hover.texture = getTexture(ToggleHoverTex);
		toggleStyle.active.texture = getTexture(ToggleActiveTex);
		toggleStyle.normalOn.texture = getTexture(ToggleNormalOnTex);
		toggleStyle.hoverOn.texture = getTexture(ToggleHoverOnTex);
		toggleStyle.activeOn.texture = getTexture(ToggleActiveOnTex);
		toggleStyle.fixedHeight = true;
		toggleStyle.fixedWidth = true;
		toggleStyle.height = 15;
		toggleStyle.width = 15;

		mSkin.setStyle(GUIToggle::getGUITypeName(), toggleStyle);

		// Color
		GUIElementStyle colorStyle;
		colorStyle.margins.left = 2;
		colorStyle.margins.right = 2;
		colorStyle.margins.top = 2;
		colorStyle.margins.bottom = 2;
		colorStyle.fixedHeight = true;
		colorStyle.height = 10;
		colorStyle.minWidth = 10;

		mSkin.setStyle(GUIColor::getGUITypeName(), colorStyle);

		// Window background texture
		GUIElementStyle windowBgStyle;
		windowBgStyle.normal.texture = getTexture(WindowBackgroundTexture);

		mSkin.setStyle("WindowBackground", windowBgStyle);

		// Window title bar background
		GUIElementStyle titleBarBgStyle;
		titleBarBgStyle.normal.texture = getTexture(WindowTitleBarBg);
		titleBarBgStyle.fixedHeight = true;
		titleBarBgStyle.height = 13;

		mSkin.setStyle("TitleBarBackground", titleBarBgStyle);

		// Tabbed title bar tab button
		GUIElementStyle tabbedBarButton;
		tabbedBarButton.normal.texture = getTexture(TabbedBarBtnNormal);
		tabbedBarButton.hover.texture = getTexture(TabbedBarBtnActive);
		tabbedBarButton.active.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.normalOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.hoverOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.activeOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.fixedHeight = true;
		tabbedBarButton.height = 13;
		tabbedBarButton.minWidth = 10;
		tabbedBarButton.maxWidth = 110;
		tabbedBarButton.font = font;
		tabbedBarButton.fontSize = DefaultFontSize;
		tabbedBarButton.textHorzAlign = THA_Center;
		tabbedBarButton.textVertAlign = TVA_Center;

		mSkin.setStyle("TabbedBarBtn", tabbedBarButton);

		// Tabbed title bar drag/drop button
		GUIElementStyle tabbedBarDropButton;
		tabbedBarDropButton.fixedHeight = true;
		tabbedBarDropButton.fixedWidth = true;
		tabbedBarDropButton.height = 13;
		tabbedBarDropButton.width = 6;

		mSkin.setStyle("TabbedBarDropArea", tabbedBarDropButton);

		// Window minimize button
		GUIElementStyle winMinButtonStyle;
		winMinButtonStyle.normal.texture = getTexture(WindowMinButtonNormal);
		winMinButtonStyle.hover.texture = getTexture(WindowMinButtonHover);
		winMinButtonStyle.active.texture = winMinButtonStyle.hover.texture;
		winMinButtonStyle.fixedHeight = true;
		winMinButtonStyle.fixedWidth = true;
		winMinButtonStyle.height = 7;
		winMinButtonStyle.width = 8;

		mSkin.setStyle("WinMinimizeBtn", winMinButtonStyle);

		// Window maximize button
		GUIElementStyle winMaxButtonStyle;
		winMaxButtonStyle.normal.texture = getTexture(WindowMaxButtonNormal);
		winMaxButtonStyle.hover.texture = getTexture(WindowMaxButtonHover);
		winMaxButtonStyle.active.texture = winMaxButtonStyle.hover.texture;
		winMaxButtonStyle.fixedHeight = true;
		winMaxButtonStyle.fixedWidth = true;
		winMaxButtonStyle.height = 8;
		winMaxButtonStyle.width = 8;

		mSkin.setStyle("WinMaximizeBtn", winMaxButtonStyle);

		// Window close button
		GUIElementStyle winCloseButtonStyle;
		winCloseButtonStyle.normal.texture = getTexture(WindowCloseButtonNormal);
		winCloseButtonStyle.hover.texture = getTexture(WindowCloseButtonHover);
		winCloseButtonStyle.active.texture = winCloseButtonStyle.hover.texture;
		winCloseButtonStyle.fixedHeight = true;
		winCloseButtonStyle.fixedWidth = true;
		winCloseButtonStyle.height = 7;
		winCloseButtonStyle.width = 8;

		mSkin.setStyle("WinCloseBtn", winCloseButtonStyle);

		// Input box
		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = getTexture(InputBoxNormalTex);
		inputBoxStyle.hover.texture = getTexture(InputBoxHoverTex);
		inputBoxStyle.focused.texture = getTexture(InputBoxFocusedTex);
		inputBoxStyle.active.texture = inputBoxStyle.normal.texture;
		inputBoxStyle.border.left = 5;
		inputBoxStyle.border.right = 5;
		inputBoxStyle.border.top = 5;
		inputBoxStyle.border.bottom = 5;
		inputBoxStyle.margins.left = 4;
		inputBoxStyle.margins.right = 4;
		inputBoxStyle.margins.top = 4;
		inputBoxStyle.margins.bottom = 4;
		inputBoxStyle.contentOffset.left = 2;
		inputBoxStyle.contentOffset.right = 2;
		inputBoxStyle.fixedHeight = true;
		inputBoxStyle.height = 21;
		inputBoxStyle.minWidth = 10;
		inputBoxStyle.font = font;
		inputBoxStyle.fontSize = DefaultFontSize;
		inputBoxStyle.textHorzAlign = THA_Left;
		inputBoxStyle.textVertAlign = TVA_Top;

		mSkin.setStyle(GUIInputBox::getGUITypeName(), inputBoxStyle);

		// Foldout
		GUIElementStyle foldoutBtnStyle;
		foldoutBtnStyle.normal.texture = getTexture(FoldoutClosedNormalTex);
		foldoutBtnStyle.hover.texture = getTexture(FoldoutClosedHoverTex);
		foldoutBtnStyle.active.texture = foldoutBtnStyle.hover.texture;
		foldoutBtnStyle.normalOn.texture = getTexture(FoldoutOpenNormalTex);
		foldoutBtnStyle.hoverOn.texture = getTexture(FoldoutOpenHoverTex);
		foldoutBtnStyle.activeOn.texture = foldoutBtnStyle.hoverOn.texture;
		foldoutBtnStyle.fixedHeight = true;
		foldoutBtnStyle.fixedWidth = true;
		foldoutBtnStyle.height = 8;
		foldoutBtnStyle.width = 8;

		mSkin.setStyle("FoldoutButton", foldoutBtnStyle);

		GUIElementStyle foldoutBackgroundStyle;
		foldoutBackgroundStyle.normal.texture = getTexture(FoldoutBackgroundTex);

		mSkin.setStyle("FoldoutBackground", foldoutBackgroundStyle);

		/************************************************************************/
		/* 								SCROLL BAR                      		*/
		/************************************************************************/

		// Up button
		GUIElementStyle scrollUpBtnStyle;
		scrollUpBtnStyle.normal.texture = getTexture(ScrollBarUpNormalTex);
		scrollUpBtnStyle.hover.texture = getTexture(ScrollBarUpHoverTex);
		scrollUpBtnStyle.active.texture = getTexture(ScrollBarUpActiveTex);
		scrollUpBtnStyle.fixedHeight = true;
		scrollUpBtnStyle.fixedWidth = true;
		scrollUpBtnStyle.height = 4;
		scrollUpBtnStyle.width = 8;

		mSkin.setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = getTexture(ScrollBarDownNormalTex);
		scrollDownBtnStyle.hover.texture = getTexture(ScrollBarDownHoverTex);
		scrollDownBtnStyle.active.texture = getTexture(ScrollBarDownActiveTex);
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		mSkin.setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = getTexture(ScrollBarLeftNormalTex);
		scrollLeftBtnStyle.hover.texture = getTexture(ScrollBarLeftHoverTex);
		scrollLeftBtnStyle.active.texture = getTexture(ScrollBarLeftActiveTex);
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		mSkin.setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = getTexture(ScrollBarRightNormalTex);
		scrollRightBtnStyle.hover.texture = getTexture(ScrollBarRightHoverTex);
		scrollRightBtnStyle.active.texture = getTexture(ScrollBarRightActiveTex);
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		mSkin.setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = getTexture(ScrollBarHandleHorzNormalTex);
		scrollBarHorzBtnStyle.hover.texture = getTexture(ScrollBarHandleHorzHoverTex);
		scrollBarHorzBtnStyle.active.texture = getTexture(ScrollBarHandleHorzActiveTex);
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		mSkin.setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = getTexture(ScrollBarHandleVertNormalTex);
		scrollBarVertBtnStyle.hover.texture = getTexture(ScrollBarHandleVertHoverTex);
		scrollBarVertBtnStyle.active.texture = getTexture(ScrollBarHandleVertActiveTex);
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		mSkin.setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

		HSpriteTexture scrollBarBgPtr = getTexture(ScrollBarBgTex);

		// Vertical scroll bar
		GUIElementStyle vertScrollBarStyle;
		vertScrollBarStyle.normal.texture = scrollBarBgPtr;
		vertScrollBarStyle.hover.texture = scrollBarBgPtr;
		vertScrollBarStyle.active.texture = scrollBarBgPtr;
		vertScrollBarStyle.fixedHeight = false;
		vertScrollBarStyle.fixedWidth = true;
		vertScrollBarStyle.minHeight = 16;
		vertScrollBarStyle.width = 8;

		mSkin.setStyle("ScrollBarVert", vertScrollBarStyle);

		// Horizontal scroll bar
		GUIElementStyle horzScrollBarStyle;
		horzScrollBarStyle.normal.texture = scrollBarBgPtr;
		horzScrollBarStyle.hover.texture = scrollBarBgPtr;
		horzScrollBarStyle.active.texture = scrollBarBgPtr;
		horzScrollBarStyle.fixedHeight = true;
		horzScrollBarStyle.fixedWidth = false;
		horzScrollBarStyle.minWidth = 16;
		horzScrollBarStyle.height = 8;

		mSkin.setStyle("ScrollBarHorz", horzScrollBarStyle);

		/************************************************************************/
		/* 								DROP DOWN BOX                      		*/
		/************************************************************************/

		// ListBox button
		GUIElementStyle dropDownListStyle;
		dropDownListStyle.normal.texture = getTexture(DropDownBtnNormalTex);
		dropDownListStyle.hover.texture = getTexture(DropDownBtnHoverTex);
		dropDownListStyle.active.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.normalOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.hoverOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.activeOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.fixedHeight = true;
		dropDownListStyle.fixedWidth = false;
		dropDownListStyle.height = 13;
		dropDownListStyle.width = 30;
		dropDownListStyle.contentOffset.left = 3;
		dropDownListStyle.contentOffset.right = 11;
		dropDownListStyle.contentOffset.top = 1;
		dropDownListStyle.contentOffset.bottom = 1;
		dropDownListStyle.border.left = 1;
		dropDownListStyle.border.right = 10;
		dropDownListStyle.border.top = 1;
		dropDownListStyle.border.bottom = 1;
		dropDownListStyle.font = font;
		dropDownListStyle.fontSize = DefaultFontSize;
		dropDownListStyle.textHorzAlign = THA_Left;
		dropDownListStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBox", dropDownListStyle);

		// DropDown scroll up button arrow
		HTexture dropDownBtnScrollUpArrow = getTexture(DropDownBoxBtnUpArrowTex);

		GUIElementStyle dropDownScrollUpBtnArrowStyle;
		dropDownScrollUpBtnArrowStyle.normal.texture = getTexture(DropDownBoxBtnUpArrowTex);
		dropDownScrollUpBtnArrowStyle.hover.texture = dropDownScrollUpBtnArrowStyle.normal.texture;
		dropDownScrollUpBtnArrowStyle.active.texture = dropDownScrollUpBtnArrowStyle.hover.texture;
		dropDownScrollUpBtnArrowStyle.fixedHeight = true;
		dropDownScrollUpBtnArrowStyle.fixedWidth = false;
		dropDownScrollUpBtnArrowStyle.height = 7;
		dropDownScrollUpBtnArrowStyle.width = 30;
		dropDownScrollUpBtnArrowStyle.border.left = 1;
		dropDownScrollUpBtnArrowStyle.border.right = 1;
		dropDownScrollUpBtnArrowStyle.border.top = 1;
		dropDownScrollUpBtnArrowStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		mSkin.setStyle("MenuBarScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		mSkin.setStyle("ContextMenuScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);

		// DropDown scroll up button
		GUIElementStyle dropDownScrollUpBtnStyle;
		dropDownScrollUpBtnStyle.normal.texture = getTexture(DropDownBoxBtnUpNormalTex);
		dropDownScrollUpBtnStyle.hover.texture = getTexture(DropDownBoxBtnUpHoverTex);
		dropDownScrollUpBtnStyle.active.texture = dropDownScrollUpBtnStyle.hover.texture;
		dropDownScrollUpBtnStyle.fixedHeight = true;
		dropDownScrollUpBtnStyle.fixedWidth = false;
		dropDownScrollUpBtnStyle.height = 7;
		dropDownScrollUpBtnStyle.width = 30;
		dropDownScrollUpBtnStyle.border.left = 1;
		dropDownScrollUpBtnStyle.border.right = 1;
		dropDownScrollUpBtnStyle.border.top = 1;
		dropDownScrollUpBtnStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollUpBtn", dropDownScrollUpBtnStyle);
		mSkin.setStyle("MenuBarScrollUpBtn", dropDownScrollUpBtnStyle);
		mSkin.setStyle("ContextMenuScrollUpBtn", dropDownScrollUpBtnStyle);

		// DropDown scroll down button arrow
		GUIElementStyle dropDownScrollDownBtnArrowStyle;
		dropDownScrollDownBtnArrowStyle.normal.texture = getTexture(DropDownBoxBtnDownArrowTex);
		dropDownScrollDownBtnArrowStyle.hover.texture = dropDownScrollDownBtnArrowStyle.normal.texture;
		dropDownScrollDownBtnArrowStyle.active.texture = dropDownScrollDownBtnArrowStyle.hover.texture;
		dropDownScrollDownBtnArrowStyle.fixedHeight = true;
		dropDownScrollDownBtnArrowStyle.fixedWidth = false;
		dropDownScrollDownBtnArrowStyle.height = 7;
		dropDownScrollDownBtnArrowStyle.width = 30;
		dropDownScrollDownBtnArrowStyle.border.left = 1;
		dropDownScrollDownBtnArrowStyle.border.right = 1;
		dropDownScrollDownBtnArrowStyle.border.top = 1;
		dropDownScrollDownBtnArrowStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		mSkin.setStyle("MenuBarScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		mSkin.setStyle("ContextMenuScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);

		// DropDown scroll down button
		GUIElementStyle dropDownScrollDownBtnStyle;
		dropDownScrollDownBtnStyle.normal.texture = getTexture(DropDownBoxBtnDownNormalTex);
		dropDownScrollDownBtnStyle.hover.texture = getTexture(DropDownBoxBtnDownHoverTex);
		dropDownScrollDownBtnStyle.active.texture = dropDownScrollDownBtnStyle.hover.texture;
		dropDownScrollDownBtnStyle.fixedHeight = true;
		dropDownScrollDownBtnStyle.fixedWidth = false;
		dropDownScrollDownBtnStyle.height = 7;
		dropDownScrollDownBtnStyle.width = 30;
		dropDownScrollDownBtnStyle.border.left = 1;
		dropDownScrollDownBtnStyle.border.right = 1;
		dropDownScrollDownBtnStyle.border.top = 1;
		dropDownScrollDownBtnStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollDownBtn", dropDownScrollDownBtnStyle);
		mSkin.setStyle("MenuBarScrollDownBtn", dropDownScrollDownBtnStyle);
		mSkin.setStyle("ContextMenuScrollDownBtn", dropDownScrollDownBtnStyle);

		// DropDown entry button
		GUIElementStyle dropDownEntryBtnStyle;
		dropDownEntryBtnStyle.normal.texture = getTexture(DropDownBoxEntryNormalTex);
		dropDownEntryBtnStyle.hover.texture = getTexture(DropDownBoxEntryHoverTex);
		dropDownEntryBtnStyle.active.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.fixedHeight = true;
		dropDownEntryBtnStyle.fixedWidth = false;
		dropDownEntryBtnStyle.height = 14;
		dropDownEntryBtnStyle.width = 30;
		dropDownEntryBtnStyle.border.left = 1;
		dropDownEntryBtnStyle.border.right = 1;
		dropDownEntryBtnStyle.border.top = 1;
		dropDownEntryBtnStyle.border.bottom = 1;
		dropDownEntryBtnStyle.font = font;
		dropDownEntryBtnStyle.fontSize = DefaultFontSize;
		dropDownEntryBtnStyle.textHorzAlign = THA_Left;
		dropDownEntryBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBoxEntryBtn", dropDownEntryBtnStyle);
		mSkin.setStyle("MenuBarEntryBtn", dropDownEntryBtnStyle);
		mSkin.setStyle("ContextMenuEntryBtn", dropDownEntryBtnStyle);

		// DropDown entry button with expand
		GUIElementStyle dropDownEntryExpBtnStyle;
		dropDownEntryExpBtnStyle.normal.texture = getTexture(DropDownBoxEntryExpNormalTex);
		dropDownEntryExpBtnStyle.hover.texture = getTexture(DropDownBoxEntryExpHoverTex);
		dropDownEntryExpBtnStyle.active.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.fixedHeight = true;
		dropDownEntryExpBtnStyle.fixedWidth = false;
		dropDownEntryExpBtnStyle.height = 14;
		dropDownEntryExpBtnStyle.width = 30;
		dropDownEntryExpBtnStyle.border.left = 1;
		dropDownEntryExpBtnStyle.border.right = 6;
		dropDownEntryExpBtnStyle.border.top = 1;
		dropDownEntryExpBtnStyle.border.bottom = 1;
		dropDownEntryExpBtnStyle.font = font;
		dropDownEntryExpBtnStyle.fontSize = DefaultFontSize;
		dropDownEntryExpBtnStyle.textHorzAlign = THA_Left;
		dropDownEntryExpBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBoxEntryExpBtn", dropDownEntryExpBtnStyle);
		mSkin.setStyle("MenuBarEntryExpBtn", dropDownEntryExpBtnStyle);
		mSkin.setStyle("ContextMenuEntryExpBtn", dropDownEntryExpBtnStyle);

		// DropDown box frame
		GUIElementStyle dropDownBoxStyle;
		dropDownBoxStyle.normal.texture = getTexture(DropDownBoxBgTex);
		dropDownBoxStyle.hover.texture = dropDownEntryBtnStyle.normal.texture;
		dropDownBoxStyle.active.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownBoxStyle.fixedHeight = false;
		dropDownBoxStyle.fixedWidth = false;
		dropDownBoxStyle.border.left = 1;
		dropDownBoxStyle.border.right = 1;
		dropDownBoxStyle.border.top = 1;
		dropDownBoxStyle.border.bottom = 1;
		dropDownBoxStyle.margins.left = 1;
		dropDownBoxStyle.margins.right = 1;
		dropDownBoxStyle.margins.top = 1;
		dropDownBoxStyle.margins.bottom = 1;

		mSkin.setStyle("ListBoxFrame", dropDownBoxStyle);
		mSkin.setStyle("MenuBarFrame", dropDownBoxStyle);
		mSkin.setStyle("ContextMenuFrame", dropDownBoxStyle);

		// Drop down separator
		GUIElementStyle dropDownSeparatorStyle;
		dropDownSeparatorStyle.normal.texture = getTexture(DropDownSeparatorTex);
		dropDownSeparatorStyle.fixedHeight = true;
		dropDownSeparatorStyle.fixedWidth = false;
		dropDownSeparatorStyle.height = 3;
		dropDownSeparatorStyle.width = 30;
		dropDownSeparatorStyle.border.left = 1;
		dropDownSeparatorStyle.border.right = 1;
		dropDownSeparatorStyle.border.top = 1;
		dropDownSeparatorStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxSeparator", dropDownSeparatorStyle);
		mSkin.setStyle("MenuBarSeparator", dropDownSeparatorStyle);
		mSkin.setStyle("ContextMenuSeparator", dropDownSeparatorStyle);

		/************************************************************************/
		/* 								MENU BAR	                     		*/
		/************************************************************************/

		// MenuBar background
		GUIElementStyle menuBarBgStyle;
		menuBarBgStyle.normal.texture = getTexture(MenuBarBgTex);
		menuBarBgStyle.fixedHeight = false;
		menuBarBgStyle.fixedWidth = false;
		menuBarBgStyle.height = 4;
		menuBarBgStyle.width = 4;

		mSkin.setStyle("MenuBarBg", menuBarBgStyle);

		// MenuBar Banshee logo
		GUIElementStyle menuBarBansheeLogoStyle;
		menuBarBansheeLogoStyle.normal.texture = getTexture(MenuBarBansheeLogoTex);
		menuBarBansheeLogoStyle.fixedHeight = true;
		menuBarBansheeLogoStyle.fixedWidth = true;
		menuBarBansheeLogoStyle.height = 7;
		menuBarBansheeLogoStyle.width = 51;

		mSkin.setStyle("MenuBarBansheeLogo", menuBarBansheeLogoStyle);

		// MenuBar button
		GUIElementStyle menuBarBtnStyle;
		menuBarBtnStyle.normal.texture = getTexture(MenuBarBtnNormalTex);
		menuBarBtnStyle.hover.texture = getTexture(MenuBarBtnHoverTex);
		menuBarBtnStyle.active.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.normalOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.hoverOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.activeOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.fixedHeight = true;
		menuBarBtnStyle.fixedWidth = false;
		menuBarBtnStyle.height = 15;
		menuBarBtnStyle.width = 4;
		menuBarBtnStyle.margins.left = 2;
		menuBarBtnStyle.margins.right = 2;
		menuBarBtnStyle.margins.top = 2;
		menuBarBtnStyle.margins.bottom = 2;
		menuBarBtnStyle.font = font;
		menuBarBtnStyle.fontSize = DefaultFontSize;
		menuBarBtnStyle.textHorzAlign = THA_Left;
		menuBarBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("MenuBarBtn", menuBarBtnStyle);

		/************************************************************************/
		/* 								DOCK SLIDER	                     		*/
		/************************************************************************/

		GUIElementStyle dockSliderBtnStyle;
		dockSliderBtnStyle.normal.texture = getTexture(DockSliderNormalTex);
		dockSliderBtnStyle.fixedHeight = false;
		dockSliderBtnStyle.fixedWidth = false;
		dockSliderBtnStyle.height = 2;
		dockSliderBtnStyle.width = 2;

		mSkin.setStyle("DockSliderBtn", dockSliderBtnStyle);

		/************************************************************************/
		/* 								TREE VIEW	                     		*/
		/************************************************************************/

		// Expand button
		GUIElementStyle treeViewExpandButtonStyle;
		treeViewExpandButtonStyle.normal.texture = getTexture(TreeViewExpandButtonOffNormal);
		treeViewExpandButtonStyle.hover.texture = getTexture(TreeViewExpandButtonOffHover);
		treeViewExpandButtonStyle.active.texture = treeViewExpandButtonStyle.hover.texture;
		treeViewExpandButtonStyle.normalOn.texture = getTexture(TreeViewExpandButtonOnNormal);
		treeViewExpandButtonStyle.hoverOn.texture = getTexture(TreeViewExpandButtonOnHover);
		treeViewExpandButtonStyle.activeOn.texture = treeViewExpandButtonStyle.hoverOn.texture;
		treeViewExpandButtonStyle.margins.left = 4;
		treeViewExpandButtonStyle.margins.right = 4;
		treeViewExpandButtonStyle.margins.top = 5;
		treeViewExpandButtonStyle.margins.bottom = 4;
		treeViewExpandButtonStyle.fixedHeight = true;
		treeViewExpandButtonStyle.fixedWidth = true;
		treeViewExpandButtonStyle.height = 16;
		treeViewExpandButtonStyle.width = 16;

		mSkin.setStyle("TreeViewFoldoutBtn", treeViewExpandButtonStyle);

		// Entry
		GUIElementStyle treeViewEntryStyle;
		treeViewEntryStyle.font = font;
		treeViewEntryStyle.fontSize = DefaultFontSize;
		treeViewEntryStyle.fixedWidth = false;
		treeViewEntryStyle.fixedHeight = true;
		treeViewEntryStyle.height = 16;
		treeViewEntryStyle.minWidth = 10;

		mSkin.setStyle("TreeViewElementBtn", treeViewEntryStyle);

		// Selection background
		GUIElementStyle treeViewSelBackgroundStyle;
		treeViewSelBackgroundStyle.normal.texture = getTexture(TreeViewSelectionBackground);
		treeViewSelBackgroundStyle.fixedHeight = false;
		treeViewSelBackgroundStyle.fixedWidth = false;
		treeViewSelBackgroundStyle.height = 2;
		treeViewSelBackgroundStyle.width = 2;

		mSkin.setStyle("TreeViewSelectionBackground", treeViewSelBackgroundStyle);

		// Edit box
		GUIElementStyle treeViewEditBox;
		treeViewEditBox.normal.texture = getTexture(TreeViewEditBox);
		treeViewEditBox.hover.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.focused.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.active.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.border.left = 1;
		treeViewEditBox.border.right = 1;
		treeViewEditBox.border.top = 1;
		treeViewEditBox.border.bottom = 1;
		treeViewEditBox.margins.left = 1;
		treeViewEditBox.margins.right = 1;
		treeViewEditBox.margins.top = 1;
		treeViewEditBox.margins.bottom = 1;
		treeViewEditBox.fixedHeight = true;
		treeViewEditBox.height = 13;
		treeViewEditBox.minWidth = 10;
		treeViewEditBox.font = font;
		treeViewEditBox.fontSize = DefaultFontSize;
		treeViewEditBox.textHorzAlign = THA_Left;
		treeViewEditBox.textVertAlign = TVA_Top;

		mSkin.setStyle(GUITreeViewEditBox::getGUITypeName(), treeViewEditBox);

		// Element highlight
		GUIElementStyle treeViewElementHighlight;
		treeViewElementHighlight.normal.texture = getTexture(TreeViewElementHighlight);
		treeViewElementHighlight.border.left = 1;
		treeViewElementHighlight.border.right = 1;
		treeViewElementHighlight.border.top = 1;
		treeViewElementHighlight.border.bottom = 1;

		mSkin.setStyle("TreeViewElementHighlight", treeViewElementHighlight);

		// Element separator highlight
		GUIElementStyle treeViewElementSepHighlight;
		treeViewElementSepHighlight.normal.texture = getTexture(TreeViewElementSepHighlight);
		treeViewElementSepHighlight.border.left = 1;
		treeViewElementSepHighlight.border.right = 1;
		treeViewElementSepHighlight.border.top = 1;
		treeViewElementSepHighlight.border.bottom = 1;

		mSkin.setStyle("TreeViewElementSepHighlight", treeViewElementSepHighlight);
	}

	HSpriteTexture EditorGUI::getTexture(const CM::WString& name)
	{
		return SpriteTexture::create(static_resource_cast<Texture>(Importer::instance().import(FileSystem::getWorkingDirectoryPath() + L"\\" + DefaultFolder + name)));
	}
}