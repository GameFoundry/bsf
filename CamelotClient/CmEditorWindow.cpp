#include "CmEditorWindow.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmSceneObject.h"
#include "CmCursor.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUILabel.h"
#include "BsGUISkin.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsUpdateCallback.h"
#include "BsEngineGUI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindow::EditorWindow(const String& name)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 200;
		renderWindowDesc.height = 200;
		renderWindowDesc.title = "EditorWindow";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.toolWindow = true;

		mRenderWindow = RenderWindow::create(renderWindowDesc, gApplication().getPrimaryWindow());

		HSceneObject so = SceneObject::create("EditorWindow-" + name);
		mGUI = so->addComponent<GUIWidget>();
		GUIManager::instance().attachWidgetToWindow(mRenderWindow.get(), mGUI.get());

		GameObjectHandle<UpdateCallback> updateCallback = so->addComponent<UpdateCallback>();

		updateCallback->onUpdate.connect(boost::bind(&EditorWindow::update, this));

		HCamera camera = so->addComponent<Camera>();
		camera->init(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		camera->setNearClipDistance(5);
		camera->setAspectRatio(1.0f);
		camera->setIgnoreSceneRenderables(true);
		
		//// DEBUG ONLY - Skin should exist externally
		//mSkin = CM_NEW(GUISkin, GUIAlloc) GUISkin();

		OverlayManager::instance().attachOverlay(camera.get(), mGUI.get());		

		//GUIElementStyle labelStyle;
		//labelStyle.font = dbgFont;
		//labelStyle.fontSize = dbgFontSize;

		//mSkin->setStyle(GUILabel::getGUITypeName(), labelStyle);

		//gui->setSkin(mSkin);
		//// END DEBUG
		mGUI->setSkin(&EngineGUI::instance().getSkin());
		mDbgLabel = GUILabel::create(mGUI.get(), "Testing test", renderWindowDesc.width);
	}

	EditorWindow::~EditorWindow()
	{
		GUIManager::instance().detachWidgetFromWindow(mRenderWindow.get(), mGUI.get());
		mRenderWindow->destroy();
	}

	void EditorWindow::update()
	{
		Int2 cursorPos = Cursor::getWindowPosition(*mRenderWindow);
		
		mDbgLabel->setText(toString(mDbgLabel->getBounds().x) + ", " + toString(mDbgLabel->getBounds().y) + ", " + 
			toString(mDbgLabel->getBounds().width) + " - " + toString(mDbgLabel->getBounds().height));
	}
}