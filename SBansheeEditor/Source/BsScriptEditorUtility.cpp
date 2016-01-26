//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptEditorUtility.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsEditorUtility.h"
#include "BsScriptSceneObject.h"
#include "BsScriptResource.h"
#include "BsScriptResourceManager.h"
#include "BsUtility.h"

namespace BansheeEngine
{
	ScriptEditorUtility::ScriptEditorUtility(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorUtility::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CalculateBounds", &ScriptEditorUtility::internal_CalculateBounds);
		metaData.scriptClass->addInternalCall("Internal_CalculateBoundsArray", &ScriptEditorUtility::internal_CalculateBoundsArray);
		metaData.scriptClass->addInternalCall("Internal_FindDependencies", &ScriptEditorUtility::internal_FindDependencies);
	}

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

	MonoArray* ScriptEditorUtility::internal_FindDependencies(MonoObject* resource, bool recursive)
	{
		ScriptResource* srcResource = ScriptResource::toNative(resource);
		if (srcResource == nullptr)
		{
			ScriptArray emptyArray = ScriptArray::create<ScriptResource>(0);
			return emptyArray.getInternal();
		}

		Vector<ResourceDependency> dependencies = Utility::findResourceDependencies(srcResource->getGenericHandle(), recursive);

		UINT32 numEntries = (UINT32)dependencies.size();
		ScriptArray output = ScriptArray::create<ScriptResource>(numEntries);
		for (UINT32 i = 0; i < numEntries; i++)
		{
			ScriptResourceBase* dependency;
			ScriptResourceManager::instance().getScriptResource(dependencies[i].resource, &dependency, true);

			output.set(i, dependency->getManagedInstance());
		}

		return output.getInternal();
	}
}