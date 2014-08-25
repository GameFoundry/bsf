#include "BsSceneEditorWidget.h"
#include "BsEditorWindow.h"
#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUIScrollArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsEditorWidgetManager.h"
#include "BsRenderTexture.h"
#include "BsSceneObject.h"
#include "BsSceneCameraController.h"
#include "BsCamera.h"
#include "BsGUIRenderTexture.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	SceneEditorWidget* SceneEditorWidget::Instance = nullptr;

	SceneEditorWidget::SceneEditorWidget(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer)
		:EditorWidget<SceneEditorWidget>(HString(L"SceneEditorWidget"), parentContainer), mGUIRenderTexture(nullptr)
	{
		updateRenderTexture(getWidth(), getHeight());
	}

	SceneEditorWidget::~SceneEditorWidget()
	{

	}

	void SceneEditorWidget::_update()
	{

	}

	void SceneEditorWidget::doOnResized(UINT32 width, UINT32 height)
	{
		EditorWidget::doOnResized(width, height);

		updateRenderTexture(width, height);
	}

	void SceneEditorWidget::updateRenderTexture(UINT32 width, UINT32 height)
	{
		width = std::max(20U, width);
		height = std::max(20U, height);

		if (mSceneRenderTarget != nullptr)
			mSceneRenderTarget->destroy();

		mSceneRenderTarget = RenderTexture::create(TEX_TYPE_2D, width, height);
		gCoreAccessor().setPriority(mSceneRenderTarget, 1);

		if (mCamera == nullptr)
		{
			HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
			mCamera = sceneCameraSO->addComponent<Camera>(mSceneRenderTarget, 0.0f, 0.0f, 1.0f, 1.0f);

			sceneCameraSO->setPosition(Vector3(0, 0, 0));
			sceneCameraSO->lookAt(Vector3(0, 0, -3));

			// DEBUG ONLY
			sceneCameraSO->setPosition(Vector3(-130.0f, 140.0f, 650.0f));
			sceneCameraSO->lookAt(Vector3(0, 0, 0));

			mCamera->setPriority(1);
			mCamera->setNearClipDistance(5);

			sceneCameraSO->addComponent<SceneCameraController>();

			GUILayout& layout = mContent->getLayout();

			mGUIRenderTexture = GUIRenderTexture::create(mSceneRenderTarget);
			layout.addElement(mGUIRenderTexture);
		}
		else
		{
			mCamera->getViewport()->setTarget(mSceneRenderTarget);
			mGUIRenderTexture->setRenderTexture(mSceneRenderTarget);
		}

		// TODO - Consider only doing the resize once user stops resizing the widget in order to reduce constant
		// render target destroy/create cycle for every little pixel.

		mCamera->setAspectRatio(width / (float)height);
	}

	SceneEditorWidget* SceneEditorWidget::instance()
	{
		return Instance;
	}

	SceneEditorWidget* SceneEditorWidget::open()
	{
		return static_cast<SceneEditorWidget*>(EditorWidgetManager::instance().open(getTypeName()));
	}

	void SceneEditorWidget::close()
	{
		if (Instance != nullptr)
			EditorWidgetManager::instance().close(Instance);

		Instance = nullptr;
	}

	const String& SceneEditorWidget::getTypeName()
	{
		static String name = "SceneEditorWidget";
		return name;
	}
}