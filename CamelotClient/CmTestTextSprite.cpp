#include "CmTestTextSprite.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "BsTextSprite.h"
#include "CmFont.h"
#include "CmMaterial.h"
#include "BsGUILabel.h"
#include "BsGUIWindowFrame.h"
#include "BsGUISkin.h"
#include "BsOverlayManager.h"
#include "BsSpriteTexture.h"

using namespace BansheeEngine;

namespace CamelotFramework
{
	TestTextSprite::TestTextSprite(const HSceneObject& parent)
		:GUIWidget(parent), mSkin(nullptr)
	{
	}

	TestTextSprite::~TestTextSprite()
	{
		if(mSkin != nullptr)
			CM_DELETE(mSkin, GUISkin, PoolAlloc);
	}

	void TestTextSprite::init(const HCamera& camera, const String& text, HFont font, UINT32 fontSize, const HTexture& windowFrameTex)
	{
		mSkin = CM_NEW(GUISkin, PoolAlloc) GUISkin();

		OverlayManager::instance().attachOverlay(camera.get(), this);		

		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = fontSize;

		mSkin->setStyle(GUILabel::getGUITypeName(), labelStyle);

		setSkin(mSkin);
		GUILabel::create(this, text, 400, 400, true, THA_Right, TVA_Bottom);

		SpriteTexturePtr frameSpriteTex(CM_NEW(SpriteTexture, PoolAlloc) SpriteTexture(windowFrameTex), &MemAllocDeleter<SpriteTexture, PoolAlloc>::deleter);
		GUIWindowFrame::create(this, frameSpriteTex);
	}

	void TestTextSprite::update()
	{

	}
}