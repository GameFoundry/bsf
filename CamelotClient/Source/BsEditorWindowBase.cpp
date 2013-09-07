#include "BsEditorWindowBase.h"
#include "CmApplication.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"
#include "CmRenderWindowManager.h"

#include "BsEditorWindowManager.h"
#include "BsCamera.h"
#include "BsGUIWindowFrameWidget.h"
#include "BsEngineGUI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindowBase::EditorWindowBase()
		:mOwnsRenderWindow(true)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 200;
		renderWindowDesc.height = 200;
		renderWindowDesc.title = "EditorWindow";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.toolWindow = true;

		mRenderWindow = RenderWindow::create(renderWindowDesc, gApplication().getPrimaryWindow());

		construct(mRenderWindow);
	}

	EditorWindowBase::EditorWindowBase(CM::RenderWindowPtr renderWindow)
		:mOwnsRenderWindow(false)
	{
		construct(renderWindow);
	}

	EditorWindowBase::~EditorWindowBase()
	{
		mMoveOrResizeConn.disconnect();

		if(mOwnsRenderWindow)
			mRenderWindow->destroy();

		mSceneObject->destroy();
	}

	void EditorWindowBase::initialize()
	{
		setPosition(0, 0);
		setSize(200, 200);
	}

	void EditorWindowBase::close()
	{
		EditorWindowManager::instance().destroy(this);
	}

	void EditorWindowBase::hide()
	{
		gMainSyncedCA().hideWindow(mRenderWindow);
	}

	void EditorWindowBase::construct(CM::RenderWindowPtr renderWindow)
	{
		mRenderWindow = renderWindow;
		mSceneObject = SceneObject::create("EditorWindow");

		mCamera = mSceneObject->addComponent<Camera>();
		mCamera->initialize(renderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(1.0f);
		mCamera->setIgnoreSceneRenderables(true);

		mGUI = mSceneObject->addComponent<GUIWidget>();
		mGUI->initialize(mCamera->getViewport().get(), renderWindow.get());
		mGUI->setDepth(128);

		mGUI->setSkin(&EngineGUI::instance().getSkin());

		mWindowFrame = mSceneObject->addComponent<WindowFrameWidget>();
		mWindowFrame->setSkin(&EngineGUI::instance().getSkin());
		mWindowFrame->initialize(mCamera->getViewport().get(), renderWindow.get());
		mWindowFrame->setDepth(129);

		mMoveOrResizeConn = RenderWindowManager::instance().onMovedOrResized.connect(boost::bind(&EditorWindowBase::movedOrResized, this, _1));
	}

	void EditorWindowBase::movedOrResized(RenderWindow& renderWindow)
	{
		if(&renderWindow == mRenderWindow.get())
			movedOrResized();
	}

	void EditorWindowBase::setPosition(CM::INT32 x, CM::INT32 y)
	{
		gMainCA().moveWindow(mRenderWindow, x, y);
	}

	void EditorWindowBase::setSize(CM::UINT32 width, CM::UINT32 height)
	{
		gMainCA().resizeWindow(mRenderWindow, width, height);
	}

	INT32 EditorWindowBase::getLeft() const
	{
		return mRenderWindow->getLeft();
	}

	INT32 EditorWindowBase::getTop() const
	{
		return mRenderWindow->getTop();
	}

	UINT32 EditorWindowBase::getWidth() const
	{
		return (UINT32) mRenderWindow->getWidth();
	}

	UINT32 EditorWindowBase::getHeight() const
	{
		return (UINT32) mRenderWindow->getHeight();
	}
}