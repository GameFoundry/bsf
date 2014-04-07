#include "BsBuiltinResources.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsTextSprite.h"
#include "BsSpriteTexture.h"

#include "CmFont.h"
#include "CmFontImportOptions.h"
#include "CmImporter.h"
#include "CmRTTIType.h"
#include "CmFileSystem.h"
#include "CmApplication.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const WString BuiltinResources::DefaultFontPath = L"arial.ttf";
	const UINT32 BuiltinResources::DefaultFontSize = 10;

	const WString BuiltinResources::DefaultSkinFolder = L"..\\..\\..\\..\\Data\\Engine\\Skin\\";
	const WString BuiltinResources::DefaultCursorFolder = L"..\\..\\..\\..\\Data\\Engine\\Cursors\\";

	const WString BuiltinResources::WhiteTex = L"White.psd";

	const WString BuiltinResources::ButtonNormalTex = L"ButtonNormal.psd";
	const WString BuiltinResources::ButtonHoverTex = L"ButtonHover.psd";

	const WString BuiltinResources::InputBoxNormalTex = L"InputBoxNormal.psd";
	const WString BuiltinResources::InputBoxHoverTex = L"InputBoxHover.psd";
	const WString BuiltinResources::InputBoxFocusedTex = L"InputBoxFocused.psd";

	const WString BuiltinResources::ScrollBarUpNormalTex = L"ScrollBarUpNormal.psd";
	const WString BuiltinResources::ScrollBarUpHoverTex = L"ScrollBarUpHover.psd";
	const WString BuiltinResources::ScrollBarUpActiveTex = L"ScrollBarUpActive.psd";

	const WString BuiltinResources::ScrollBarDownNormalTex = L"ScrollBarDownNormal.psd";
	const WString BuiltinResources::ScrollBarDownHoverTex = L"ScrollBarDownHover.psd";
	const WString BuiltinResources::ScrollBarDownActiveTex = L"ScrollBarDownActive.psd";

	const WString BuiltinResources::ScrollBarLeftNormalTex = L"ScrollBarLeftNormal.psd";
	const WString BuiltinResources::ScrollBarLeftHoverTex = L"ScrollBarLeftHover.psd";
	const WString BuiltinResources::ScrollBarLeftActiveTex = L"ScrollBarLeftActive.psd";

	const WString BuiltinResources::ScrollBarRightNormalTex = L"ScrollBarRightNormal.psd";
	const WString BuiltinResources::ScrollBarRightHoverTex = L"ScrollBarRightHover.psd";
	const WString BuiltinResources::ScrollBarRightActiveTex = L"ScrollBarRightActive.psd";

	const WString BuiltinResources::ScrollBarHandleHorzNormalTex = L"ScrollBarHorzHandleNormal.psd";
	const WString BuiltinResources::ScrollBarHandleHorzHoverTex = L"ScrollBarHorzHandleHover.psd";
	const WString BuiltinResources::ScrollBarHandleHorzActiveTex = L"ScrollBarHorzHandleActive.psd";

	const WString BuiltinResources::ScrollBarHandleVertNormalTex = L"ScrollBarVertHandleNormal.psd";
	const WString BuiltinResources::ScrollBarHandleVertHoverTex = L"ScrollBarVertHandleHover.psd";
	const WString BuiltinResources::ScrollBarHandleVertActiveTex = L"ScrollBarVertHandleActive.psd";

	const WString BuiltinResources::DropDownBtnNormalTex = L"DropDownNormal.psd";
	const WString BuiltinResources::DropDownBtnHoverTex = L"DropDownHover.psd";

	const WString BuiltinResources::DropDownBoxBgTex = L"DropDownBoxBg.psd";
	const WString BuiltinResources::DropDownBoxEntryNormalTex = L"DropDownButtonNormal.psd";
	const WString BuiltinResources::DropDownBoxEntryHoverTex = L"DropDownButtonHover.psd";

	const WString BuiltinResources::DropDownBoxBtnUpNormalTex = L"DropDownBoxBtnUpNormal.psd";
	const WString BuiltinResources::DropDownBoxBtnUpHoverTex = L"DropDownBoxBtnUpHover.psd";

	const WString BuiltinResources::DropDownBoxBtnDownNormalTex = L"DropDownBoxBtnDownNormal.psd";
	const WString BuiltinResources::DropDownBoxBtnDownHoverTex = L"DropDownBoxBtnDownHover.psd";

	const WString BuiltinResources::DropDownBoxEntryExpNormalTex = L"DropDownExpNormal.psd";
	const WString BuiltinResources::DropDownBoxEntryExpHoverTex = L"DropDownExpHover.psd";

	const WString BuiltinResources::DropDownSeparatorTex = L"DropDownSeparator.psd";

	const WString BuiltinResources::DropDownBoxBtnUpArrowTex = L"DropDownBoxBtnUpArrow.psd";
	const WString BuiltinResources::DropDownBoxBtnDownArrowTex = L"DropDownBoxBtnDownArrow.psd";

	const WString BuiltinResources::ScrollBarBgTex = L"ScrollBarBg.psd";

	const WString BuiltinResources::CursorArrowTex = L"Arrow.psd";
	const WString BuiltinResources::CursorArrowDragTex = L"ArrowDrag.psd";
	const WString BuiltinResources::CursorArrowLeftRightTex = L"ArrowLeftRight.psd";
	const WString BuiltinResources::CursorIBeamTex = L"IBeam.psd";
	const WString BuiltinResources::CursorDenyTex = L"Deny.psd";
	const WString BuiltinResources::CursorWaitTex = L"Wait.psd";
	const WString BuiltinResources::CursorSizeNESWTex = L"SizeNESW.psd";
	const WString BuiltinResources::CursorSizeNSTex = L"SizeNS.psd";
	const WString BuiltinResources::CursorSizeNWSETex = L"SizeNWSE.psd";
	const WString BuiltinResources::CursorSizeWETex = L"SizeWE.psd";

	const Vector2I BuiltinResources::CursorArrowHotspot = Vector2I(11, 8);
	const Vector2I BuiltinResources::CursorArrowDragHotspot = Vector2I(11, 8);
	const Vector2I BuiltinResources::CursorArrowLeftRightHotspot = Vector2I(9, 4);
	const Vector2I BuiltinResources::CursorIBeamHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorDenyHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorWaitHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNESWHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNSHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNWSEHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeWEHotspot = Vector2I(15, 15);

	BuiltinResources::BuiltinResources()
	{
		// TODO - Normally I want to load this from some file
		
		mWhiteSpriteTexture = getSkinTexture(WhiteTex);

		/************************************************************************/
		/* 								CURSOR		                     		*/
		/************************************************************************/

		HTexture cursorArrowTex = getCursorTexture(CursorArrowTex);
		HTexture cursorArrowDragTex = getCursorTexture(CursorArrowDragTex);
		HTexture cursorArrowLeftRightTex = getCursorTexture(CursorArrowLeftRightTex);
		HTexture cursorIBeamTex = getCursorTexture(CursorIBeamTex);
		HTexture cursorDenyTex = getCursorTexture(CursorDenyTex);
		HTexture cursorWaitTex = getCursorTexture(CursorWaitTex);
		HTexture cursorSizeNESWTex = getCursorTexture(CursorSizeNESWTex);
		HTexture cursorSizeNSTex = getCursorTexture(CursorSizeNSTex);
		HTexture cursorSizeNWSETex = getCursorTexture(CursorSizeNWSETex);
		HTexture cursorSizeWETex = getCursorTexture(CursorSizeWETex);

		mCursorArrow = cursorArrowTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowTex.getInternalPtr(), 0, mCursorArrow);

		mCursorArrowDrag = cursorArrowDragTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowDragTex.getInternalPtr(), 0, mCursorArrowDrag);

		mCursorArrowLeftRight = cursorArrowLeftRightTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowLeftRightTex.getInternalPtr(), 0, mCursorArrowLeftRight);

		mCursorIBeam = cursorIBeamTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorIBeamTex.getInternalPtr(), 0, mCursorIBeam);

		mCursorDeny = cursorDenyTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorDenyTex.getInternalPtr(), 0, mCursorDeny);

		mCursorWait = cursorWaitTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorWaitTex.getInternalPtr(), 0, mCursorWait);

		mCursorSizeNESW = cursorSizeNESWTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNESWTex.getInternalPtr(), 0, mCursorSizeNESW);

		mCursorSizeNS = cursorSizeNSTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNSTex.getInternalPtr(), 0, mCursorSizeNS);

		mCursorSizeNWSE = cursorSizeNWSETex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNWSETex.getInternalPtr(), 0, mCursorSizeNWSE);

		mCursorSizeWE = cursorSizeWETex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeWETex.getInternalPtr(), 0, mCursorSizeWE);

		gCoreAccessor().submitToCoreThread(true);

		// Label
		// TODO - Instead of importing font every time, try to save a resource and then just load it?
		HFont font;

		{
			WString fontPath = DefaultSkinFolder + DefaultFontPath;
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

		// Button
		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = getSkinTexture(ButtonNormalTex);
		buttonStyle.hover.texture = getSkinTexture(ButtonHoverTex);
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

		// Input box
		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = getSkinTexture(InputBoxNormalTex);
		inputBoxStyle.hover.texture = getSkinTexture(InputBoxHoverTex);
		inputBoxStyle.focused.texture = getSkinTexture(InputBoxFocusedTex);
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
		scrollUpBtnStyle.normal.texture = getSkinTexture(ScrollBarUpNormalTex);
		scrollUpBtnStyle.hover.texture = getSkinTexture(ScrollBarUpHoverTex);
		scrollUpBtnStyle.active.texture = getSkinTexture(ScrollBarUpActiveTex);
		scrollUpBtnStyle.fixedHeight = true;
		scrollUpBtnStyle.fixedWidth = true;
		scrollUpBtnStyle.height = 4;
		scrollUpBtnStyle.width = 8;

		mSkin.setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = getSkinTexture(ScrollBarDownNormalTex);
		scrollDownBtnStyle.hover.texture = getSkinTexture(ScrollBarDownHoverTex);
		scrollDownBtnStyle.active.texture = getSkinTexture(ScrollBarDownActiveTex);
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		mSkin.setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = getSkinTexture(ScrollBarLeftNormalTex);
		scrollLeftBtnStyle.hover.texture = getSkinTexture(ScrollBarLeftHoverTex);
		scrollLeftBtnStyle.active.texture = getSkinTexture(ScrollBarLeftActiveTex);
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		mSkin.setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = getSkinTexture(ScrollBarRightNormalTex);
		scrollRightBtnStyle.hover.texture = getSkinTexture(ScrollBarRightHoverTex);
		scrollRightBtnStyle.active.texture = getSkinTexture(ScrollBarRightActiveTex);
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		mSkin.setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = getSkinTexture(ScrollBarHandleHorzNormalTex);
		scrollBarHorzBtnStyle.hover.texture = getSkinTexture(ScrollBarHandleHorzHoverTex);
		scrollBarHorzBtnStyle.active.texture = getSkinTexture(ScrollBarHandleHorzActiveTex);
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		mSkin.setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = getSkinTexture(ScrollBarHandleVertNormalTex);
		scrollBarVertBtnStyle.hover.texture = getSkinTexture(ScrollBarHandleVertHoverTex);
		scrollBarVertBtnStyle.active.texture = getSkinTexture(ScrollBarHandleVertActiveTex);
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		mSkin.setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

		HSpriteTexture scrollBarBgPtr = getSkinTexture(ScrollBarBgTex);

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
		dropDownListStyle.normal.texture = getSkinTexture(DropDownBtnNormalTex);
		dropDownListStyle.hover.texture = getSkinTexture(DropDownBtnHoverTex);
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
		GUIElementStyle dropDownScrollUpBtnArrowStyle;
		dropDownScrollUpBtnArrowStyle.normal.texture = getSkinTexture(DropDownBoxBtnUpArrowTex);
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
		dropDownScrollUpBtnStyle.normal.texture = getSkinTexture(DropDownBoxBtnUpNormalTex);
		dropDownScrollUpBtnStyle.hover.texture = getSkinTexture(DropDownBoxBtnUpHoverTex);
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
		dropDownScrollDownBtnArrowStyle.normal.texture = getSkinTexture(DropDownBoxBtnDownArrowTex);
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
		dropDownScrollDownBtnStyle.normal.texture = getSkinTexture(DropDownBoxBtnDownNormalTex);
		dropDownScrollDownBtnStyle.hover.texture = getSkinTexture(DropDownBoxBtnDownHoverTex);
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
		dropDownEntryBtnStyle.normal.texture = getSkinTexture(DropDownBoxEntryNormalTex);
		dropDownEntryBtnStyle.hover.texture = getSkinTexture(DropDownBoxEntryHoverTex);
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
		dropDownEntryExpBtnStyle.normal.texture = getSkinTexture(DropDownBoxEntryExpNormalTex);
		dropDownEntryExpBtnStyle.hover.texture = getSkinTexture(DropDownBoxEntryExpHoverTex);
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
		dropDownBoxStyle.normal.texture = getSkinTexture(DropDownBoxBgTex);
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
		dropDownSeparatorStyle.normal.texture = getSkinTexture(DropDownSeparatorTex);
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
	}

	HSpriteTexture BuiltinResources::getSkinTexture(const CM::WString& name)
	{
		return SpriteTexture::create(static_resource_cast<Texture>(Importer::instance().import(FileSystem::getWorkingDirectoryPath() + L"\\" + DefaultSkinFolder + name)));
	}

	HTexture BuiltinResources::getCursorTexture(const CM::WString& name)
	{
		return static_resource_cast<Texture>(Importer::instance().import(FileSystem::getWorkingDirectoryPath() + L"\\" + DefaultCursorFolder + name));
	}

	const CM::PixelData& BuiltinResources::getCursorArrow(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowHotspot;
		return *mCursorArrow.get();
	}

	const CM::PixelData& BuiltinResources::getCursorArrowDrag(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowDragHotspot;
		return *mCursorArrowDrag.get();
	}

	const CM::PixelData& BuiltinResources::getCursorWait(Vector2I& hotSpot)
	{
		hotSpot = CursorWaitHotspot;
		return *mCursorWait.get();
	}

	const CM::PixelData& BuiltinResources::getCursorIBeam(Vector2I& hotSpot)
	{
		hotSpot = CursorIBeamHotspot;
		return *mCursorIBeam.get();
	}

	const CM::PixelData& BuiltinResources::getCursorSizeNESW(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNESWHotspot;
		return *mCursorSizeNESW.get();
	}

	const CM::PixelData& BuiltinResources::getCursorSizeNS(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNSHotspot;
		return *mCursorSizeNS.get();
	}

	const CM::PixelData& BuiltinResources::getCursorSizeNWSE(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNWSEHotspot;
		return *mCursorSizeNWSE.get();
	}

	const CM::PixelData& BuiltinResources::getCursorSizeWE(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeWEHotspot;
		return *mCursorSizeWE.get();
	}

	const CM::PixelData& BuiltinResources::getCursorDeny(Vector2I& hotSpot)
	{
		hotSpot = CursorDenyHotspot;
		return *mCursorDeny.get();
	}

	const CM::PixelData& BuiltinResources::getCursorMoveLeftRight(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowLeftRightHotspot;
		return *mCursorArrowLeftRight.get();
	}
}