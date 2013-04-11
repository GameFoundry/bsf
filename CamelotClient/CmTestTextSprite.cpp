#include "CmTestTextSprite.h"
#include "CmSceneObject.h"
#include "CmRenderable.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "CmTextSprite.h"
#include "CmFont.h"
#include "CmMaterial.h"
#include "CmGUILabel.h"
#include "CmGUISkin.h"
#include "CmOverlayManager.h"

namespace CamelotEngine
{
	TestTextSprite::TestTextSprite(const HSceneObject& parent)
		:GUIWidget(parent), mSkin(nullptr)
	{
	}

	TestTextSprite::~TestTextSprite()
	{
		if(mSkin != nullptr)
			CM_DELETE(mSkin, GUISkin, GUIAlloc);
	}

	void TestTextSprite::setText(const HCamera& camera, const String& text, HFont font, UINT32 fontSize)
	{
		mSkin = CM_NEW(GUISkin, GUIAlloc) GUISkin();

		OverlayManager::instance().attachOverlay(camera.get(), this);		

		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = fontSize;

		mSkin->setStyle(GUILabel::getGUITypeName(), labelStyle);

		setSkin(mSkin);
		addLabel(text, 400, 400, true, THA_Right, TVA_Bottom);
	}

	void TestTextSprite::update()
	{

	}
}