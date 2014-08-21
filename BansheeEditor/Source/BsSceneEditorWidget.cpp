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

		UINT32 width = 200; // TODO - Use proper width/height once I have texture resize working. Have some mininum limit (e.g. 4x4)
		UINT32 height = 200;
		
		mSceneRenderTarget = RenderTexture::create(TEX_TYPE_2D, width, height);
		mSceneRenderTarget->setPriority(1);

		HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
		mCamera = sceneCameraSO->addComponent<Camera>(mSceneRenderTarget, 0.0f, 0.0f, 1.0f, 1.0f);

		sceneCameraSO->setPosition(Vector3(0, 0, 0));
		sceneCameraSO->lookAt(Vector3(0, 0, -3));

		// DEBUG ONLY
		sceneCameraSO->setPosition(Vector3(-130.0f, 140.0f, 650.0f));
		sceneCameraSO->lookAt(Vector3(0, 0, 0));

		mCamera->setPriority(1);
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(width / (float)height);

		sceneCameraSO->addComponent<SceneCameraController>();

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