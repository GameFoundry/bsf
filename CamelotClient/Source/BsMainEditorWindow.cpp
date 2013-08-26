#include "BsMainEditorWindow.h"
#include "BsDockManager.h"
#include "BsCamera.h"
#include "CmSceneObject.h"
#include "CmRenderTexture.h"

// DEBUG ONLY
#include "CmTestTextSprite.h"
#include "CmDebugCamera.h"
#include "BsDrawHelper.h"
#include "CmFRect.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	MainEditorWindow::MainEditorWindow(CM::RenderWindowPtr renderWindow)
		:EditorWindowBase(renderWindow), mDockManager(cm_new<DockManager>(mGUI.get()))
	{
		// DEBUG ONLY

		HSceneObject sceneCameraGO = SceneObject::create("SceneCamera");
		HCamera sceneCamera = sceneCameraGO->addComponent<Camera>();

		//RenderTexturePtr sceneRenderTarget = RenderTexture::create(TEX_TYPE_2D, 800, 600);

		sceneCamera->initialize(mCamera->getViewport()->getTarget(), 0.0f, 0.0f, 1.0f, 1.0f, 0);
		sceneCamera->setPriority(-1);
		sceneCameraGO->setPosition(Vector3(0,50,1240));
		sceneCameraGO->lookAt(Vector3(0,50,-300));
		sceneCamera->setNearClipDistance(5);
		sceneCamera->setAspectRatio(800.0f / 600.0f);

		GameObjectHandle<DebugCamera> debugCamera = sceneCameraGO->addComponent<DebugCamera>();

		GameObjectHandle<TestTextSprite> textSprite = mSceneObject->addComponent<TestTextSprite>();
		textSprite->initialize(mCamera->getViewport().get(), renderWindow.get());

		textSprite->init(sceneCamera, "Testing in a new row, does this work?", nullptr);

		DrawHelper::instance().drawQuad2D(sceneCamera, FRect(0.0f, 0.2f, 0.75f, 0.5f), Color::White, CoordType::Normalized, 250.0f);
		DrawHelper::instance().drawQuad2D(sceneCamera, FRect(50.0f, 50.0f, 100.0f, 50.0f), Color::Blue, CoordType::Pixel, 250.0f);

		DrawHelper::instance().drawLine2D_Pixel(sceneCamera, Vector2(0, 0), Vector2(20, 20), Color::Blue, CoordType::Pixel, 250.0f);

		DrawHelper::instance().drawLine2D_AA(sceneCamera, Vector2(100, 0), Vector2(120, 20), 5.0f, Color::Blue, CoordType::Pixel, 250.0f);
	}

	MainEditorWindow::~MainEditorWindow()
	{
		cm_delete(mDockManager);
	}

	void MainEditorWindow::movedOrResized()
	{
		EditorWindowBase::movedOrResized();

		UINT32 widgetWidth = (UINT32)std::max(0, (INT32)getWidth() - 2);
		UINT32 widgetHeight = (UINT32)std::max(0, (INT32)getHeight() - 2);

		mDockManager->setArea(1, 1, widgetWidth, widgetHeight);
	}
}