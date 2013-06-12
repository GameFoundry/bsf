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

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String EngineGUI::DefaultFontPath = "C:\\arial.ttf";
	const UINT32 EngineGUI::DefaultFontSize = 10;

	const String EngineGUI::WindowBackgroundTexture = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowBgTile.psd";

	const String EngineGUI::ButtonNormalTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\ButtonNormal.psd";
	const String EngineGUI::ButtonHoverTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\ButtonHover.psd";

	const String EngineGUI::WindowFrameNormal = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowFrameNormal.psd";
	const String EngineGUI::WindowFrameFocused = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowFrameFocused.psd";

	const String EngineGUI::WindowTitleBarBg = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowTitleBarBg.psd";

	const String EngineGUI::WindowCloseButtonNormal = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowCloseBtnNormal.psd";
	const String EngineGUI::WindowCloseButtonHover = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowCloseBtnHover.psd";

	const String EngineGUI::WindowMinButtonNormal = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowMinBtnNormal.psd";
	const String EngineGUI::WindowMinButtonHover = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\WindowMinBtnHover.psd";

	const String EngineGUI::TabbedBarBtnNormal = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\TabbedButtonNormal.psd";
	const String EngineGUI::TabbedBarBtnActive = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\TabbedButtonActive.psd";

	const String EngineGUI::InputBoxNormalTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\InputBoxNormal.psd";
	const String EngineGUI::InputBoxHoverTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\InputBoxHover.psd";
	const String EngineGUI::InputBoxFocusedTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\InputBoxFocused.psd";

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
		HTexture windowFrameNormalTex = static_resource_cast<Texture>(Importer::instance().import(WindowFrameNormal));
		HTexture windowFrameFocusedTex = static_resource_cast<Texture>(Importer::instance().import(WindowFrameFocused));

		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowFrameNormalTex));
		windowFrameStyle.focused.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowFrameFocusedTex));
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		mSkin.setStyle("WindowFrame", windowFrameStyle);

		// Button
		HTexture buttonNormalTex = static_resource_cast<Texture>(Importer::instance().import(ButtonNormalTex));
		HTexture buttonHoverTex = static_resource_cast<Texture>(Importer::instance().import(ButtonHoverTex));

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
		buttonStyle.minWidth = 10;
		buttonStyle.font = font;
		buttonStyle.fontSize = DefaultFontSize;
		buttonStyle.textHorzAlign = THA_Center;
		buttonStyle.textVertAlign = TVA_Center;

		mSkin.setStyle(GUIButton::getGUITypeName(), buttonStyle);

		// Window background texture
		HTexture windowBgTexture = static_resource_cast<Texture>(Importer::instance().import(WindowBackgroundTexture));

		GUIElementStyle windowBgStyle;
		windowBgStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowBgTexture));

		mSkin.setStyle("WindowBackground", windowBgStyle);

		// Window title bar background
		HTexture windowTitleBarBg = static_resource_cast<Texture>(Importer::instance().import(WindowTitleBarBg));

		GUIElementStyle titleBarBgStyle;
		titleBarBgStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(windowTitleBarBg));
		titleBarBgStyle.fixedHeight = true;
		titleBarBgStyle.height = 13;

		mSkin.setStyle("TitleBarBackground", titleBarBgStyle);

		// Tabbed title bar tab button
		HTexture tabbedBarBtnNormal = static_resource_cast<Texture>(Importer::instance().import(TabbedBarBtnNormal));
		HTexture tabbedBarBtnActive = static_resource_cast<Texture>(Importer::instance().import(TabbedBarBtnActive));

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
		HTexture winMinBtnNormal = static_resource_cast<Texture>(Importer::instance().import(WindowMinButtonNormal));
		HTexture winMinBtnHover = static_resource_cast<Texture>(Importer::instance().import(WindowMinButtonHover));

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
		HTexture winCloseBtnNormal = static_resource_cast<Texture>(Importer::instance().import(WindowCloseButtonNormal));
		HTexture winCloseBtnHover = static_resource_cast<Texture>(Importer::instance().import(WindowCloseButtonHover));

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
		HTexture inputBoxNormalTex = static_resource_cast<Texture>(Importer::instance().import(InputBoxNormalTex));
		HTexture inputBoxHoverTex = static_resource_cast<Texture>(Importer::instance().import(InputBoxHoverTex));
		HTexture inputBoxFocusedTex = static_resource_cast<Texture>(Importer::instance().import(InputBoxFocusedTex));

		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxNormalTex));
		inputBoxStyle.hover.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxHoverTex));
		inputBoxStyle.focused.texture = cm_shared_ptr<SpriteTexture, PoolAlloc>(std::cref(inputBoxFocusedTex));
		inputBoxStyle.active.texture = inputBoxStyle.focused.texture;
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
	}

}