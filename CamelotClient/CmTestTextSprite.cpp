#include "CmTestTextSprite.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "BsTextSprite.h"
#include "CmFont.h"
#include "CmMaterial.h"
#include "BsGUILabel.h"
#include "BsGUISkin.h"
#include "BsOverlayManager.h"
#include "BsSpriteTexture.h"
#include "BsEngineGUI.h"

using namespace BansheeEngine;

namespace CamelotFramework
{
	TestTextSprite::TestTextSprite(const HSceneObject& parent)
		:GUIWidget(parent)
	{
	}

	TestTextSprite::~TestTextSprite()
	{
	}

	void TestTextSprite::init(const HCamera& camera, const String& text)
	{
		setSkin(&EngineGUI::instance().getSkin());

		GUILabel::create(*this, text);
	}

	void TestTextSprite::update()
	{

	}
}