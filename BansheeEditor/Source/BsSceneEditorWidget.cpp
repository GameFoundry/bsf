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

namespace BansheeEngine
{
	SceneEditorWidget* SceneEditorWidget::Instance = nullptr;

	SceneEditorWidget::SceneEditorWidget(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer)
		:EditorWidget<SceneEditorWidget>(HString(L"SceneEditorWidget"), parentContainer)
	{
		GUILayout& layout = mContent->getLayout();
		
		mSceneRenderTarget = RenderTexture::create(TEX_TYPE_2D, getWidth(), getHeight());
		mSceneRenderTarget->setPriority(1);

		HSceneObject sceneCameraGO = SceneObject::create("SceneCamera");
		mCamera = sceneCameraGO->addComponent<Camera>(mSceneRenderTarget, 0.0f, 0.0f, 1.0f, 1.0f);

		sceneCameraGO->setPosition(Vector3(0, 0, 0));
		sceneCameraGO->lookAt(Vector3(0, 0, -3));

		mCamera->setPriority(1);
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(getWidth() / (float)getHeight());

		sceneCameraGO->addComponent<SceneCameraController>();

		layout.addElement(GUIRenderTexture::create(mSceneRenderTarget));
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

		// TODO - Need to resize render texture as the widget resizes

		//mSceneRenderTarget->set
		//mCamera->setAspectRatio(getWidth() / getHeight());
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