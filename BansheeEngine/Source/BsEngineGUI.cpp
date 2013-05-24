#include "BsEngineGUI.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIWindowFrame.h"
#include "BsGUIButton.h"
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

	const String EngineGUI::WindowFramePrimaryTexture = "C:\\WindowFrame.psd";

	const String EngineGUI::ButtonNormalTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\ButtonNormal.psd";
	const String EngineGUI::ButtonHoverTex = "C:\\Projects\\BansheeEngine\\Data\\Editor\\Skin\\ButtonHover.psd";

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

				vector<CamelotFramework::UINT32>::type fontSizes;
				fontSizes.push_back(DefaultFontSize);
				importOptions->setFontSizes(fontSizes);
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
		HTexture windowFrameTex = static_resource_cast<Texture>(Importer::instance().import(WindowFramePrimaryTexture));
		windowFrameTex.waitUntilLoaded();

		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = SpriteTexturePtr(CM_NEW(SpriteTexture, PoolAlloc) SpriteTexture(windowFrameTex), &MemAllocDeleter<SpriteTexture, PoolAlloc>::deleter);
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		mSkin.setStyle(GUIWindowFrame::getGUITypeName(), windowFrameStyle);

		// Button
		HTexture buttonNormalTex = static_resource_cast<Texture>(Importer::instance().import(ButtonNormalTex));
		HTexture buttonHoverTex = static_resource_cast<Texture>(Importer::instance().import(ButtonHoverTex));
		buttonNormalTex.waitUntilLoaded();
		buttonHoverTex.waitUntilLoaded();

		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = SpriteTexturePtr(CM_NEW(SpriteTexture, PoolAlloc) SpriteTexture(buttonNormalTex), &MemAllocDeleter<SpriteTexture, PoolAlloc>::deleter);
		buttonStyle.hover.texture = SpriteTexturePtr(CM_NEW(SpriteTexture, PoolAlloc) SpriteTexture(buttonHoverTex), &MemAllocDeleter<SpriteTexture, PoolAlloc>::deleter);
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
	}

}