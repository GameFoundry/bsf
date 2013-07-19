#include "CmEditorWindow.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmSceneObject.h"
#include "CmCursor.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUILabel.h"
#include "BsGUIWindowFrameWidget.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUITexture.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsUpdateCallback.h"
#include "BsEngineGUI.h"
#include "BsGUIArea.h"
#include "BsGUITabbedTitleBar.h"
#include "BsGUIScrollBarVert.h"

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
		mGUI->setDepth(128);

		mGUI->setSkin(&EngineGUI::instance().getSkin());

		GameObjectHandle<TabbedTitleBar> titleBar = so->addComponent<TabbedTitleBar>();
		titleBar->setSkin(&EngineGUI::instance().getSkin());
		titleBar->initialize(camera->getViewport().get(), mRenderWindow.get());
		titleBar->setDepth(127);

		GameObjectHandle<WindowFrameWidget> frame = so->addComponent<WindowFrameWidget>();
		frame->setSkin(&EngineGUI::instance().getSkin());
		frame->initialize(camera->getViewport().get(), mRenderWindow.get());
		frame->setDepth(129);

		//// DEBUG
		
		GUIArea* dbgArea = GUIArea::createStretchedXY(*mGUI, 1, 1, 14, 2, 475);
		GUILayout& layout = dbgArea->getLayout();
		//
		//mDbgLabel = GUILabel::create(*mGUI, "Testing test");
		//layout.addElement(mDbgLabel);

		layout.addElement(GUIInputBox::create(*mGUI));
		layout.addElement(GUIInputBox::create(*mGUI, GUILayoutOptions::fixed(100, 100), true));
		layout.addElement(GUIScrollBarVert::create(*mGUI));
		
		//GUIFlexibleSpace& space4 = otherLayout.addFlexibleSpace();
		//otherLayout.addElement(mDbgLabel);

		//GUIFixedSpace& space = otherLayout.addSpace(10); // Due to bug in MSVC compiler I need to store return value
		//GUIFlexibleSpace& space3 = otherLayout.addFlexibleSpace();
		//otherLayout.addElement(GUIWindowFrame::create(*mGUI, GUILayoutOptions::fixed(100, 100)));
		//GUIFixedSpace& space2 = otherLayout.addSpace(10);
		//otherLayout.addElement(GUIButton::create(*mGUI, "Test"));
		//otherLayout.addElement(GUIWindowFrame::create(*mGUI));
		
		//GUIArea* backgroundArea = GUIArea::create(*mGUI, 0, 0, 0, 0, 2000);
		//backgroundArea->getLayout().addElement(GUITexture::create(*mGUI, GUILayoutOptions::expandableXY(), GUIImageScaleMode::RepeatToFit, mGUI->getSkin()->getStyle("WindowBackground")));

		//GUIArea* windowFrameArea = GUIArea::create(*mGUI, 0, 0, 0, 0, 1999);
		//windowFrameArea->getLayout().addElement(GUIWindowFrame::create(*mGUI));

		//GUIArea* titleBarBackgroundArea = GUIArea::create(*mGUI, 0, 1, 0, 11, 1999);
		//titleBarBackgroundArea->getLayout().addSpace(1);
		//titleBarBackgroundArea->getLayout().addElement(GUITexture::create(*mGUI, GUIImageScaleMode::RepeatToFit, GUILayoutOptions::expandableXY(), mGUI->getGUISkin()->getStyle("TitleBarBg")));
		//titleBarBackgroundArea->getLayout().addSpace(1);
		
		//mRenderWindow->resize(300, 250);
		//mRenderWindow->setVisible(false);
	}

	EditorWindow::~EditorWindow()
	{
		mRenderWindow->destroy();
	}

	void EditorWindow::update()
	{
		//Int2 cursorPos = Cursor::getWindowPosition(*mRenderWindow);
		
		//mDbgLabel->setText("Position: " + toString(cursorPos.x) + ", " + toString(cursorPos.y));
		
	}
}