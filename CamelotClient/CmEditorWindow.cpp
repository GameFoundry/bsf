#include "CmEditorWindow.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmSceneObject.h"
#include "CmGUIWidget.h"
#include "CmGUILabel.h"
#include "CmGUISkin.h"
#include "CmOverlayManager.h"
#include "CmCamera.h"

namespace CamelotEditor
{
	EditorWindow::EditorWindow(const String& name, const HFont& dbgFont, UINT32 dbgFontSize)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 200;
		renderWindowDesc.height = 200;
		renderWindowDesc.title = "EditorWindow";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.toolWindow = true;

		mRenderWindow = RenderWindow::create(renderWindowDesc, gApplication().getPrimaryRenderWindow());

		HSceneObject so = SceneObject::create("EditorWindow-" + name);
		//HGUIWidget gui = so->addComponent<GUIWidget>();
		HCamera camera = so->addComponent<Camera>();

		camera->init(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		camera->setNearClipDistance(5);
		camera->setAspectRatio(1.0f);
		
		//// DEBUG ONLY - Skin should exist externally
		//mSkin = CM_NEW(GUISkin, GUIAlloc) GUISkin();

		//OverlayManager::instance().attachOverlay(camera.get(), gui.get());		

		//GUIElementStyle labelStyle;
		//labelStyle.font = dbgFont;
		//labelStyle.fontSize = dbgFontSize;

		//mSkin->setStyle(GUILabel::getGUITypeName(), labelStyle);

		//gui->setSkin(mSkin);
		//// END DEBUG

		//gui->addLabel("Testing test");
	}

	EditorWindow::~EditorWindow()
	{
		mRenderWindow->destroy();
	}
}