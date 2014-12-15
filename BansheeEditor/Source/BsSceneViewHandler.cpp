#include "BsSceneViewHandler.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsGizmoManager.h"
#include "BsHandleManager.h"
#include "BsSceneGrid.h"
#include "BsSelection.h"
#include "BsScenePicking.h"
#include "BsCameraHandler.h"
#include "BsEditorApplication.h"
#include "BsEditorWidget.h"
#include "BsEditorWindowBase.h"
#include "BsRenderWindow.h"
#include "BsCursor.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	SceneViewHandler::SceneViewHandler(EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera)
		:mCamera(camera), mSceneGrid(nullptr), mParentWidget(parentWidget)
	{
		mRenderCallback = RendererManager::instance().getActive()->onRenderViewport.connect(std::bind(&SceneViewHandler::render, this, _1, _2));
		mSceneGrid = bs_new<SceneGrid>();
		mSceneGrid->setSettings(gEditorApplication().getEditorSettings());
		HandleManager::instance().setSettings(gEditorApplication().getEditorSettings());
	}

	SceneViewHandler::~SceneViewHandler()
	{
		bs_delete(mSceneGrid);
		mRenderCallback.disconnect();

		if (GizmoManager::isStarted()) // If not active, we don't care
			GizmoManager::instance().clearRenderData();
	}

	void SceneViewHandler::update(const Vector2I& position, const Vector2I& delta)
	{
		if (HandleManager::instance().isHandleActive())
			wrapCursorToWindow();

		GizmoManager::instance().update(mCamera);
		HandleManager::instance().update(mCamera, position, delta);
		mSceneGrid->update();
	}

	void SceneViewHandler::trySelectHandle(const Vector2I& position)
	{
		HandleManager::instance().trySelect(mCamera, position);
	}

	bool SceneViewHandler::isHandleActive() const
	{
		return HandleManager::instance().isHandleActive();
	}

	void SceneViewHandler::clearHandleSelection()
	{
		HandleManager::instance().clearSelection();
	}

	void SceneViewHandler::pickObject(const Vector2I& position, bool additive)
	{
		// TODO - Handle multi-selection (i.e. selection rectangle when dragging)
		HSceneObject pickedObject = ScenePicking::instance().pickClosestObject(mCamera, position, Vector2I(1, 1));

		if (pickedObject)
		{
			if (additive) // Append to existing selection
			{
				Vector<HSceneObject> selectedSOs = Selection::instance().getSceneObjects();

				auto iterFind = std::find_if(selectedSOs.begin(), selectedSOs.end(),
					[&](const HSceneObject& obj) { return obj == pickedObject; }
				);

				if (iterFind != selectedSOs.end())
					selectedSOs.push_back(pickedObject);

				Selection::instance().setSceneObjects(selectedSOs);
			}
			else
			{
				Vector<HSceneObject> selectedSOs = { pickedObject };

				Selection::instance().setSceneObjects(selectedSOs);
			}
		}
		else
			Selection::instance().clearSceneSelection();
	}

	void SceneViewHandler::render(const Viewport* viewport, DrawList& drawList)
	{
		if (mCamera == nullptr)
			return;

		if (mCamera->getViewport().get() != viewport)
			return;

		mSceneGrid->render(mCamera, drawList);
	}

	void SceneViewHandler::wrapCursorToWindow()
	{
		RenderWindowPtr parentWindow = mParentWidget->getParentWindow()->_getRenderWindow();

		Vector2I windowPos = parentWindow->screenToWindowPos(Cursor::instance().getScreenPosition());
		const RenderWindowProperties& rwProps = parentWindow->getProperties();

		if (windowPos.x < 0)
			windowPos.x += rwProps.getWidth();
		else if (windowPos.x >= (INT32)rwProps.getWidth())
			windowPos.x -= rwProps.getWidth();

		if (windowPos.y < 0)
			windowPos.y += rwProps.getHeight();
		else if (windowPos.y >= (INT32)rwProps.getHeight())
			windowPos.y -= rwProps.getHeight();

		Vector2I wrappedScreenPos = parentWindow->windowToScreenPos(windowPos);
		Cursor::instance().setScreenPosition(wrappedScreenPos);
	}
}