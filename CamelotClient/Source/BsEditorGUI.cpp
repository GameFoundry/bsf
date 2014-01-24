#include "BsEditorGUI.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
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
	const String EditorGUI::DefaultFontPath = "arial.ttf";
	const UINT32 EditorGUI::DefaultFontSize = 10;

	const String EditorGUI::DefaultFolder = "..\\..\\..\\..\\Data\\Editor\\Skin\\";

	const String EditorGUI::WindowBackgroundTexture = "WindowBgTile.psd";

	const String EditorGUI::ButtonNormalTex = "ButtonNormal.psd";
	const String EditorGUI::ButtonHoverTex = "ButtonHover.psd";

	const String EditorGUI::WindowFrameNormal = "WindowFrameNormal.psd";
	const String EditorGUI::WindowFrameFocused = "WindowFrameFocused.psd";

	const String EditorGUI::WindowTitleBarBg = "WindowTitleBarBg.psd";

	const String EditorGUI::WindowCloseButtonNormal = "WindowCloseBtnNormal.psd";
	const String EditorGUI::WindowCloseButtonHover = "WindowCloseBtnHover.psd";

	const String EditorGUI::WindowMinButtonNormal = "WindowMaxBtnNormal.psd";
	const String EditorGUI::WindowMinButtonHover = "WindowMaxBtnHover.psd";

	const String EditorGUI::WindowMaxButtonNormal = "WindowMinBtnNormal.psd";
	const String EditorGUI::WindowMaxButtonHover = "WindowMinBtnHover.psd";

	const String EditorGUI::TabbedBarBtnNormal = "TabbedButtonNormal.psd";
	const String EditorGUI::TabbedBarBtnActive = "TabbedButtonActive.psd";

	const String EditorGUI::InputBoxNormalTex = "InputBoxNormal.psd";
	const String EditorGUI::InputBoxHoverTex = "InputBoxHover.psd";
	const String EditorGUI::InputBoxFocusedTex = "InputBoxFocused.psd";

	const String EditorGUI::ScrollBarUpNormalTex = "ScrollBarUpNormal.psd";
	const String EditorGUI::ScrollBarUpHoverTex = "ScrollBarUpHover.psd";
	const String EditorGUI::ScrollBarUpActiveTex = "ScrollBarUpActive.psd";

	const String EditorGUI::ScrollBarDownNormalTex = "ScrollBarDownNormal.psd";
	const String EditorGUI::ScrollBarDownHoverTex = "ScrollBarDownHover.psd";
	const String EditorGUI::ScrollBarDownActiveTex = "ScrollBarDownActive.psd";

	const String EditorGUI::ScrollBarLeftNormalTex = "ScrollBarLeftNormal.psd";
	const String EditorGUI::ScrollBarLeftHoverTex = "ScrollBarLeftHover.psd";
	const String EditorGUI::ScrollBarLeftActiveTex = "ScrollBarLeftActive.psd";

	const String EditorGUI::ScrollBarRightNormalTex = "ScrollBarRightNormal.psd";
	const String EditorGUI::ScrollBarRightHoverTex = "ScrollBarRightHover.psd";
	const String EditorGUI::ScrollBarRightActiveTex = "ScrollBarRightActive.psd";

	const String EditorGUI::ScrollBarHandleHorzNormalTex = "ScrollBarHorzHandleNormal.psd";
	const String EditorGUI::ScrollBarHandleHorzHoverTex = "ScrollBarHorzHandleHover.psd";
	const String EditorGUI::ScrollBarHandleHorzActiveTex = "ScrollBarHorzHandleActive.psd";

	const String EditorGUI::ScrollBarHandleVertNormalTex = "ScrollBarVertHandleNormal.psd";
	const String EditorGUI::ScrollBarHandleVertHoverTex = "ScrollBarVertHandleHover.psd";
	const String EditorGUI::ScrollBarHandleVertActiveTex = "ScrollBarVertHandleActive.psd";

	const String EditorGUI::DropDownBtnNormalTex = "DropDownNormal.psd";
	const String EditorGUI::DropDownBtnHoverTex = "DropDownHover.psd";

	const String EditorGUI::DropDownBoxBgTex = "DropDownBoxBg.psd";
	const String EditorGUI::DropDownBoxEntryNormalTex = "DropDownButtonNormal.psd";
	const String EditorGUI::DropDownBoxEntryHoverTex = "DropDownButtonHover.psd";

	const String EditorGUI::DropDownBoxBtnUpNormalTex = "DropDownBoxBtnUpNormal.psd";
	const String EditorGUI::DropDownBoxBtnUpHoverTex = "DropDownBoxBtnUpHover.psd";

	const String EditorGUI::DropDownBoxBtnDownNormalTex = "DropDownBoxBtnDownNormal.psd";
	const String EditorGUI::DropDownBoxBtnDownHoverTex = "DropDownBoxBtnDownHover.psd";

	const String EditorGUI::DropDownBoxEntryExpNormalTex = "DropDownExpNormal.psd";
	const String EditorGUI::DropDownBoxEntryExpHoverTex = "DropDownExpHover.psd";

	const String EditorGUI::DropDownSeparatorTex = "DropDownSeparator.psd";

	const String EditorGUI::DropDownBoxBtnUpArrowTex = "DropDownBoxBtnUpArrow.psd";
	const String EditorGUI::DropDownBoxBtnDownArrowTex = "DropDownBoxBtnDownArrow.psd";

	const String EditorGUI::ScrollBarBgTex = "ScrollBarBg.psd";

	const String EditorGUI::MenuBarBgTex = "MenuBarBg.psd";

	const String EditorGUI::MenuBarBtnNormalTex = "MenuBarButtonNormal.psd";
	const String EditorGUI::MenuBarBtnHoverTex = "MenuBarButtonHover.psd";

	const String EditorGUI::MenuBarBansheeLogoTex = "MenuBarBansheeLogo.psd";

	const String EditorGUI::DockSliderNormalTex = "DockSliderBtn.psd";

	const String EditorGUI::TreeViewExpandButtonOffNormal = "TreeViewExpandButtonOffNormal.psd";
	const String EditorGUI::TreeViewExpandButtonOffHover = "TreeViewExpandButtonOffHover.psd";
	const String EditorGUI::TreeViewExpandButtonOnNormal = "TreeViewExpandButtonOnNormal.psd";
	const String EditorGUI::TreeViewExpandButtonOnHover = "TreeViewExpandButtonOnHover.psd";

	const String EditorGUI::TreeViewSelectionBackground = "TreeViewSelectionBackground.psd";
	const String EditorGUI::TreeViewEditBox = "TreeViewEditBox.psd";

	EditorGUI::EditorGUI()
	{
		// TODO - Normally I want to load this from some file

		// Label
		// TODO - Instead of importing font every time, try to save a resource and then just load it?
		HFont font;

		{
			String fontPath = DefaultFolder + DefaultFontPath;
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
		HTexture dropDownBtnScrollUpArrow = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + DropDownBoxBtnUpArrowTex));

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
	}

	HSpriteTexture EditorGUI::getTexture(const CM::String& name)
	{
		return SpriteTexture::create(static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + DefaultFolder + name)));
	}
}