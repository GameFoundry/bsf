#include "BsEditorWindowBase.h"
#include "CmApplication.h"
#include "CmCoreThread.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"

#include "BsEditorWindowManager.h"
#include "BsCamera.h"
#include "BsGUIWindowFrameWidget.h"
#include "BsEditorGUI.h"

namespace BansheeEngine
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

	EditorWindowBase::EditorWindowBase(const RenderWindowPtr& renderWindow)
		:mOwnsRenderWindow(false)
	{
		construct(renderWindow);
	}

	EditorWindowBase::~EditorWindowBase()
	{
		mResizedConn.disconnect();

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
		gCoreAccessor().hideWindow(mRenderWindow);
	}

	void EditorWindowBase::construct(const RenderWindowPtr& renderWindow)
	{
		mRenderWindow = renderWindow;
		mSceneObject = SceneObject::create("EditorWindow");

		mCamera = mSceneObject->addComponent<Camera>();
		mCamera->initialize(renderWindow, 0.0f, 0.0f, 1.0f, 1.0f);
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(1.0f);
		mCamera->setIgnoreSceneRenderables(true);

		mGUI = mSceneObject->addComponent<GUIWidget>(mCamera->getViewport().get());
		mGUI->setDepth(128);

		mGUI->setSkin(EditorGUI::instance().getSkin());

		mWindowFrame = mSceneObject->addComponent<WindowFrameWidget>(mCamera->getViewport().get(), renderWindow.get(), EditorGUI::instance().getSkin());
		mWindowFrame->setDepth(129);

		mResizedConn = mCamera->getViewport()->onResized.connect(std::bind(&EditorWindowBase::resized, this));
	}

	void EditorWindowBase::setPosition(INT32 x, INT32 y)
	{
		gCoreAccessor().moveWindow(mRenderWindow, x, y);
	}

	void EditorWindowBase::setSize(UINT32 width, UINT32 height)
	{
		gCoreAccessor().resizeWindow(mRenderWindow, width, height);
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