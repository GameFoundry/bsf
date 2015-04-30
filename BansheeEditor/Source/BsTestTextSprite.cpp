#include "BsTestTextSprite.h"
#include "BsSceneObject.h"
#include "BsRenderable.h"
#include "BsMesh.h"
#include "BsVector2.h"
#include "BsTextSprite.h"
#include "BsFont.h"
#include "BsMaterial.h"
#include "BsGUILabel.h"
#include "BsGUIListBox.h"
#include "BsGUISkin.h"
#include "BsOverlayManager.h"
#include "BsSpriteTexture.h"
#include "BsBuiltinEditorResources.h"
#include "BsGUITexture.h"
#include "BsGUIRenderTexture.h"
#include "BsGUILayout.h"
#include "BsGUISpace.h"
#include "BsGUIViewport.h"
#include "BsGUIButton.h"
#include "BsCamera.h"
#include "BsInput.h"
#include "BsPlatform.h"
#include "BsGUIContent.h"
#include "BsStringTable.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIResourceTreeView.h"
#include "BsGUIScrollArea.h"
#include "BsGUIIntField.h"
#include "BsGUIFloatField.h"

namespace BansheeEngine
{
	TestTextSprite::TestTextSprite(const HSceneObject& parent, Viewport* target)
		:GUIWidget(parent, target)
	{
	}

	TestTextSprite::~TestTextSprite()
	{
	}

	void TestTextSprite::init(const HCamera& camera, const String& text, RenderTexturePtr sceneView)
	{
		setSkin(BansheeEngine::BuiltinEditorResources::instance().getSkin());
		setDepth(128);

		SceneObject::create("FILLER_A");
		SceneObject::create("FILLER_B");
		SceneObject::create("FILLER_C");
		SceneObject::create("FILLER_D");
		SceneObject::create("FILLER_E");
		SceneObject::create("FILLER_F");
		SceneObject::create("FILLER_G");
		SceneObject::create("FILLER_H");
		SceneObject::create("FILLER_I");
		SceneObject::create("FILLER_J");
		SceneObject::create("FILLER_K");
		SceneObject::create("FILLER_L");
	}

	void TestTextSprite::update()
	{

	}
}