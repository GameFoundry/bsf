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
#include "BsGUITexture.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUIViewport.h"
#include "BsCamera.h"

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

	void TestTextSprite::init(const HCamera& camera, const String& text, CM::RenderTexturePtr sceneView)
	{
		setSkin(&EngineGUI::instance().getSkin());

		GUIArea* area = GUIArea::createStretchedXY(*this, 0, 0, 0, 0);

		SpriteTexturePtr spriteTex = std::make_shared<SpriteTexture>(sceneView->getBindableColorTexture());
		area->getLayout().addElement(GUITexture::create(*this, GUILayoutOptions::fixed(800, 600), spriteTex));
	}

	void TestTextSprite::update()
	{

	}
}