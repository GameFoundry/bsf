#include "BsEngineGUI.h"
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

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String EngineGUI::DefaultFontPath = "C:\\arial.ttf";
	const UINT32 EngineGUI::DefaultFontSize = 10;

	const String EngineGUI::WindowBackgroundTexture = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowBgTile.psd";

	const String EngineGUI::ButtonNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ButtonNormal.psd";
	const String EngineGUI::ButtonHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ButtonHover.psd";

	const String EngineGUI::WindowFrameNormal = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowFrameNormal.psd";
	const String EngineGUI::WindowFrameFocused = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowFrameFocused.psd";

	const String EngineGUI::WindowTitleBarBg = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowTitleBarBg.psd";

	const String EngineGUI::WindowCloseButtonNormal = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowCloseBtnNormal.psd";
	const String EngineGUI::WindowCloseButtonHover = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowCloseBtnHover.psd";

	const String EngineGUI::WindowMinButtonNormal = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowMinBtnNormal.psd";
	const String EngineGUI::WindowMinButtonHover = "..\\..\\..\\..\\Data\\Editor\\Skin\\WindowMinBtnHover.psd";

	const String EngineGUI::TabbedBarBtnNormal = "..\\..\\..\\..\\Data\\Editor\\Skin\\TabbedButtonNormal.psd";
	const String EngineGUI::TabbedBarBtnActive = "..\\..\\..\\..\\Data\\Editor\\Skin\\TabbedButtonActive.psd";

	const String EngineGUI::InputBoxNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\InputBoxNormal.psd";
	const String EngineGUI::InputBoxHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\InputBoxHover.psd";
	const String EngineGUI::InputBoxFocusedTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\InputBoxFocused.psd";

	const String EngineGUI::ScrollBarUpNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarUpNormal.psd";
	const String EngineGUI::ScrollBarUpHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarUpHover.psd";
	const String EngineGUI::ScrollBarUpActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarUpActive.psd";

	const String EngineGUI::ScrollBarDownNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarDownNormal.psd";
	const String EngineGUI::ScrollBarDownHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarDownHover.psd";
	const String EngineGUI::ScrollBarDownActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarDownActive.psd";

	const String EngineGUI::ScrollBarLeftNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarLeftNormal.psd";
	const String EngineGUI::ScrollBarLeftHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarLeftHover.psd";
	const String EngineGUI::ScrollBarLeftActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarLeftActive.psd";

	const String EngineGUI::ScrollBarRightNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarRightNormal.psd";
	const String EngineGUI::ScrollBarRightHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarRightHover.psd";
	const String EngineGUI::ScrollBarRightActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarRightActive.psd";

	const String EngineGUI::ScrollBarHandleHorzNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarHorzHandleNormal.psd";
	const String EngineGUI::ScrollBarHandleHorzHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarHorzHandleHover.psd";
	const String EngineGUI::ScrollBarHandleHorzActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarHorzHandleActive.psd";

	const String EngineGUI::ScrollBarHandleVertNormalTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarVertHandleNormal.psd";
	const String EngineGUI::ScrollBarHandleVertHoverTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarVertHandleHover.psd";
	const String EngineGUI::ScrollBarHandleVertActiveTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarVertHandleActive.psd";

	const String EngineGUI::ScrollBarBgTex = "..\\..\\..\\..\\Data\\Editor\\Skin\\ScrollBarBg.psd";

	EngineGUI::EngineGUI()
	{
		// TODO - Normally I want to load this from some file
		
		// Label
		// TODO - Instead of importing font every time, try to save a resource and then just load it?
		HFont font;

		{
			ImportOptionsPtr fontImportOptions = Importer::instance().createImportOptions(DefaultFontPath);
			if(rtti_is_of_type<FontImportOptions>(fontImportOptions))
			{
				FontImportOptions* importOptions = static_cast<FontImportOptions*>(fontImportOptions.get());

				Vector<CamelotFramework::UINT32>::type fontSizes;
				fontSizes.push_back(DefaultFontSize);
				importOptions->setFontSizes(fontSizes);
				importOptions->setAntialiasing(false);
			}

			font = Importer::instance().import(DefaultFontPath, fontImportOptions);
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
		HTexture windowFrameNormalTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowFrameNormal));
		HTexture windowFrameFocusedTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowFrameFocused));

		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowFrameNormalTex));
		windowFrameStyle.focused.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowFrameFocusedTex));
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		mSkin.setStyle("WindowFrame", windowFrameStyle);

		// Button
		HTexture buttonNormalTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ButtonNormalTex));
		HTexture buttonHoverTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ButtonHoverTex));

		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(buttonNormalTex));
		buttonStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(buttonHoverTex));
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
		HTexture windowBgTexture = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowBackgroundTexture));

		GUIElementStyle windowBgStyle;
		windowBgStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowBgTexture));

		mSkin.setStyle("WindowBackground", windowBgStyle);

		// Window title bar background
		HTexture windowTitleBarBg = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowTitleBarBg));

		GUIElementStyle titleBarBgStyle;
		titleBarBgStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowTitleBarBg));
		titleBarBgStyle.fixedHeight = true;
		titleBarBgStyle.height = 13;

		mSkin.setStyle("TitleBarBackground", titleBarBgStyle);

		// Tabbed title bar tab button
		HTexture tabbedBarBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + TabbedBarBtnNormal));
		HTexture tabbedBarBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + TabbedBarBtnActive));

		GUIElementStyle tabbedBarButton;
		tabbedBarButton.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(tabbedBarBtnNormal));
		tabbedBarButton.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(tabbedBarBtnActive));
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
		HTexture winMinBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowMinButtonNormal));
		HTexture winMinBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowMinButtonHover));

		GUIElementStyle winMinButtonStyle;
		winMinButtonStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(winMinBtnNormal));
		winMinButtonStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(winMinBtnHover));
		winMinButtonStyle.active.texture = winMinButtonStyle.hover.texture;
		winMinButtonStyle.fixedHeight = true;
		winMinButtonStyle.fixedWidth = true;
		winMinButtonStyle.height = 7;
		winMinButtonStyle.width = 8;

		mSkin.setStyle("WinMinimizeBtn", winMinButtonStyle);

		// Window close button
		HTexture winCloseBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowCloseButtonNormal));
		HTexture winCloseBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + WindowCloseButtonHover));

		GUIElementStyle winCloseButtonStyle;
		winCloseButtonStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(winCloseBtnNormal));
		winCloseButtonStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(winCloseBtnHover));
		winCloseButtonStyle.active.texture = winCloseButtonStyle.active.texture;
		winCloseButtonStyle.fixedHeight = true;
		winCloseButtonStyle.fixedWidth = true;
		winCloseButtonStyle.height = 7;
		winCloseButtonStyle.width = 8;

		mSkin.setStyle("WinCloseBtn", winCloseButtonStyle);

		// Input box
		HTexture inputBoxNormalTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + InputBoxNormalTex));
		HTexture inputBoxHoverTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + InputBoxHoverTex));
		HTexture inputBoxFocusedTex = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + InputBoxFocusedTex));

		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxNormalTex));
		inputBoxStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxHoverTex));
		inputBoxStyle.focused.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxFocusedTex));
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
		HTexture scrollUpBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarUpNormalTex));
		HTexture scrollUpBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarUpHoverTex));
		HTexture scrollUpBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarUpActiveTex));

		GUIElementStyle scrollUpBtnStyle;
		scrollUpBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollUpBtnNormal));
		scrollUpBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollUpBtnHover));
		scrollUpBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollUpBtnActive));
		scrollUpBtnStyle.fixedHeight = true;
		scrollUpBtnStyle.fixedWidth = true;
		scrollUpBtnStyle.height = 4;
		scrollUpBtnStyle.width = 8;

		mSkin.setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		HTexture scrollDownBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarDownNormalTex));
		HTexture scrollDownBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarDownHoverTex));
		HTexture scrollDownBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarDownActiveTex));

		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollDownBtnNormal));
		scrollDownBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollDownBtnHover));
		scrollDownBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollDownBtnActive));
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		mSkin.setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		HTexture scrollLeftBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarLeftNormalTex));
		HTexture scrollLeftBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarLeftHoverTex));
		HTexture scrollLeftBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarLeftActiveTex));

		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollLeftBtnNormal));
		scrollLeftBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollLeftBtnHover));
		scrollLeftBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollLeftBtnActive));
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		mSkin.setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		HTexture scrollRightBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarRightNormalTex));
		HTexture scrollRightBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarRightHoverTex));
		HTexture scrollRightBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarRightActiveTex));

		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollRightBtnNormal));
		scrollRightBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollRightBtnHover));
		scrollRightBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollRightBtnActive));
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		mSkin.setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		HTexture scrollBarHorzBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleHorzNormalTex));
		HTexture scrollBarHorzBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleHorzHoverTex));
		HTexture scrollBarHorzBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleHorzActiveTex));

		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarHorzBtnNormal));
		scrollBarHorzBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarHorzBtnHover));
		scrollBarHorzBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarHorzBtnActive));
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		mSkin.setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		HTexture scrollBarVertBtnNormal = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleVertNormalTex));
		HTexture scrollBarVertBtnHover = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleVertHoverTex));
		HTexture scrollBarVertBtnActive = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarHandleVertActiveTex));

		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarVertBtnNormal));
		scrollBarVertBtnStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarVertBtnHover));
		scrollBarVertBtnStyle.active.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarVertBtnActive));
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		mSkin.setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

		HTexture scrollBarBg = static_resource_cast<Texture>(Importer::instance().import(FileSystem::getCurrentPath() + "\\" + ScrollBarBgTex));
		SpriteTexturePtr scrollBarBgPtr = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(scrollBarBg));

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
	}

}