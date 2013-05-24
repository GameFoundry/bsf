#include "CmEditorWindow.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmSceneObject.h"
#include "CmCursor.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUILabel.h"
#include "BsGUIWindowFrame.h"
#include "BsGUIButton.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsUpdateCallback.h"
#include "BsEngineGUI.h"
#include "BsGUIArea.h"

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

		GameObjectHandle<UpdateCallback> updateCallback = so->addComponent<UpdateCallback>();
		updateCallback->onUpdate.connect(boost::bind(&EditorWindow::update, this));

		HCamera camera = so->addComponent<Camera>();
		camera->initialize(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		camera->setNearClipDistance(5);
		camera->setAspectRatio(1.0f);
		camera->setIgnoreSceneRenderables(true);

		mGUI = so->addComponent<GUIWidget>();
		mGUI->initialize(camera->getViewport().get(), mRenderWindow.get());

		//// DEBUG
		mGUI->setSkin(&EngineGUI::instance().getSkin());

		//GUIArea* backgroundArea = GUIArea::create(*mGUI, 0, 0, 0, 0, 0);
		//GUILayout& layout = backgroundArea->getLayout();
		
		mDbgLabel = GUILabel::create(*mGUI, "Testing test");
		/*layout.addElement(mDbgLabel);*/

		GUIArea* mainArea = GUIArea::create(*mGUI, 0, 0, 0, 0, 1);
		GUILayout& otherLayout = mainArea->getLayout();

		//GUIFlexibleSpace& space4 = otherLayout.addFlexibleSpace();
		//otherLayout.addElement(mDbgLabel);

		//GUIFixedSpace& space = otherLayout.addSpace(10); // Due to bug in MSVC compiler I need to store return value
		//GUIFlexibleSpace& space3 = otherLayout.addFlexibleSpace();
		otherLayout.addElement(GUIWindowFrame::create(*mGUI, GUILayoutOptions::fixed(100, 100)));
		//GUIFixedSpace& space2 = otherLayout.addSpace(10);
		otherLayout.addElement(GUIButton::create(*mGUI, "Test"));
		//otherLayout.addElement(GUIWindowFrame::create(*mGUI));
	}

	EditorWindow::~EditorWindow()
	{
		mRenderWindow->destroy();
	}

	void EditorWindow::update()
	{
		Int2 cursorPos = Cursor::getWindowPosition(*mRenderWindow);
		
		mDbgLabel->setText("Position: " + toString(cursorPos.x) + ", " + toString(cursorPos.y));
	}
}