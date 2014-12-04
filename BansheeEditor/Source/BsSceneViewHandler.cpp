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

using namespace std::placeholders;

namespace BansheeEngine
{
	SceneViewHandler::SceneViewHandler(const SPtr<CameraHandler>& camera)
		:mCamera(camera), mSceneGrid(nullptr)
	{
		mRenderCallback = RendererManager::instance().getActive()->onRenderViewport.connect(std::bind(&SceneViewHandler::render, this, _1, _2));
		mSceneGrid = bs_new<SceneGrid>();
		mSceneGrid->setSettings(gEditorApplication().getEditorSettings());
	}

	SceneViewHandler::~SceneViewHandler()
	{
		bs_delete(mSceneGrid);
		mRenderCallback.disconnect();

		GizmoManager::instance().clearRenderData();
	}

	void SceneViewHandler::update(const Vector2I& position)
	{
		GizmoManager::instance().update(mCamera);
		HandleManager::instance().update(mCamera, position); // TODO - Make sure these internally check ProjectSettings
		mSceneGrid->update();
	}

	void SceneViewHandler::pointerPressed(const Vector2I& position)
	{
		HandleManager::instance().trySelect(mCamera, position);
	}

	void SceneViewHandler::pointerReleased(const Vector2I& position, bool controlHeld)
	{
		if (HandleManager::instance().isHandleActive())
			HandleManager::instance().clearSelection();
		else
		{
			// TODO - Handle multi-selection (i.e. selection rectangle when dragging)
			HSceneObject pickedObject = ScenePicking::instance().pickClosestObject(mCamera, position, Vector2I(1, 1));

			if (pickedObject)
			{
				if (controlHeld) // Append to existing selection
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
	}

	void SceneViewHandler::render(const Viewport* viewport, DrawList& drawList)
	{
		if (mCamera == nullptr)
			return;

		if (mCamera->getViewport().get() != viewport)
			return;

		mSceneGrid->render(mCamera, drawList);
	}
}