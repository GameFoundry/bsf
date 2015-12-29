#include "BsScriptSceneSelection.h"
#include "BsScriptCamera.h"
#include "BsSelectionRenderer.h"
#include "BsScenePicking.h"
#include "BsSelection.h"

namespace BansheeEngine
{
	ScriptSceneSelection::ScriptSceneSelection(MonoObject* object, const SPtr<Camera>& camera)
		:ScriptObject(object), mSelectionRenderer(nullptr), mCamera(camera)
	{
		mSelectionRenderer = bs_new<SelectionRenderer>();
	}

	ScriptSceneSelection::~ScriptSceneSelection()
	{
		bs_delete(mSelectionRenderer);
	}

	void ScriptSceneSelection::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptSceneSelection::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Draw", &ScriptSceneSelection::internal_Draw);
		metaData.scriptClass->addInternalCall("Internal_PickObject", &ScriptSceneSelection::internal_PickObject);
	}

	void ScriptSceneSelection::internal_Create(MonoObject* managedInstance, ScriptCamera* camera)
	{
		new (bs_alloc<ScriptSceneSelection>()) ScriptSceneSelection(managedInstance, camera->getInternal());
	}

	void ScriptSceneSelection::internal_Draw(ScriptSceneSelection* thisPtr)
	{
		thisPtr->mSelectionRenderer->update(thisPtr->mCamera);
	}

	void ScriptSceneSelection::internal_PickObject(ScriptSceneSelection* thisPtr, Vector2I* inputPos, bool additive)
	{
		// TODO - Handle multi-selection (i.e. selection rectangle when dragging)
		HSceneObject pickedObject = ScenePicking::instance().pickClosestObject(thisPtr->mCamera, *inputPos, Vector2I(1, 1));

		if (pickedObject)
		{
			if (additive) // Append to existing selection
			{
				Vector<HSceneObject> selectedSOs = Selection::instance().getSceneObjects();

				auto iterFind = std::find_if(selectedSOs.begin(), selectedSOs.end(),
					[&](const HSceneObject& obj) { return obj == pickedObject; }
				);

				if (iterFind == selectedSOs.end())
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