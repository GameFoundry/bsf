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

#include "BsDebug.h"

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

	void SceneViewHandler::update()
	{
		GizmoManager::instance().update(mCamera);
		mSceneGrid->update();
	}

	void SceneViewHandler::updateHandle(const Vector2I& position, const Vector2I& delta)
	{
		// If mouse wrapped around last frame then we need to compensate for the jump amount
		Vector2I realDelta = delta - mMouseDeltaCompensate;
		mMouseDeltaCompensate = Vector2I::ZERO;

		if (HandleManager::instance().isHandleActive())
			mMouseDeltaCompensate = wrapCursorToWindow();

		HandleManager::instance().update(mCamera, position, realDelta);
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

	Vector2I SceneViewHandler::wrapCursorToWindow()
	{
		RenderWindowPtr parentWindow = mParentWidget->getParentWindow()->_getRenderWindow();

		Vector2I windowPos = parentWindow->screenToWindowPos(Cursor::instance().getScreenPosition());
		const RenderWindowProperties& rwProps = parentWindow->getProperties();

		Vector2I offset;
		if (windowPos.x < 0)
			offset.x = (INT32)rwProps.getWidth();
		else if (windowPos.x >= (INT32)rwProps.getWidth())
			offset.x = -(INT32)rwProps.getWidth();

		if (windowPos.y < 0)
			offset.y = (INT32)rwProps.getHeight();
		else if (windowPos.y >= (INT32)rwProps.getHeight())
			offset.y = -(INT32)rwProps.getHeight();

		windowPos += offset;
		
		Vector2I wrappedScreenPos = parentWindow->windowToScreenPos(windowPos);
		Cursor::instance().setScreenPosition(wrappedScreenPos);

		return offset;
	}
}