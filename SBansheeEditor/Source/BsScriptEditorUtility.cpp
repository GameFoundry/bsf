#include "BsScriptEditorUtility.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsEditorUtility.h"
#include "BsScriptSceneObject.h"

namespace BansheeEngine
{
	ScriptEditorUtility::ScriptEditorUtility(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorUtility::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CalculateBounds", &ScriptEditorUtility::internal_CalculateBounds);
		metaData.scriptClass->addInternalCall("Internal_CalculateBoundsArray", &ScriptEditorUtility::internal_CalculateBoundsArray);
	}

	static void internal_CalculateBounds(MonoObject* so, AABox* bounds);
	static void internal_CalculateBounds(MonoArray* objects, AABox* bounds);

	void ScriptEditorUtility::internal_CalculateBounds(MonoObject* so, AABox* bounds)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(so);

		if (scriptSO != nullptr)
			*bounds = EditorUtility::calculateBounds(static_object_cast<SceneObject>(scriptSO->getNativeHandle()));
		else
			*bounds = AABox();
	}

	void ScriptEditorUtility::internal_CalculateBoundsArray(MonoArray* objects, AABox* bounds)
	{
		Vector<HSceneObject> sceneObjects;

		UINT32 arrayLen = (UINT32)mono_array_length(objects);
		for (UINT32 i = 0; i < arrayLen; i++)
		{
			MonoObject* curObject = mono_array_get(objects, MonoObject*, i);

			ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(curObject);

			if (scriptSO != nullptr)
				sceneObjects.push_back(static_object_cast<SceneObject>(scriptSO->getNativeHandle()));
		}

		*bounds = EditorUtility::calculateBounds(sceneObjects);
	}
}