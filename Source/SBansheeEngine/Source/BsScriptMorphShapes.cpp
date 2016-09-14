//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptMorphShapes.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsMorphShapes.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptMorphShapes::ScriptMorphShapes(MonoObject* managedInstance, const SPtr<MorphShapes>& morphShapes)
		:ScriptObject(managedInstance), mMorphShapes(morphShapes)
	{ }

	void ScriptMorphShapes::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetShapes", &ScriptMorphShapes::internal_GetShapes);
	}

	MonoObject* ScriptMorphShapes::create(const SPtr<MorphShapes>& morphShapes)
	{
		MonoObject* instance = metaData.scriptClass->createInstance();

		new (bs_alloc<ScriptMorphShapes>()) ScriptMorphShapes(instance, morphShapes);
		return instance;
	}

	MonoArray* ScriptMorphShapes::internal_GetShapes(ScriptMorphShapes* thisPtr)
	{
		SPtr<MorphShapes> morphShapes = thisPtr->getInternal();

		UINT32 numShapes = morphShapes->getNumShapes();
		ScriptArray scriptArray(MonoUtil::getStringClass(), numShapes);

		for (UINT32 i = 0; i < numShapes; i++)
		{
			MonoString* monoString = MonoUtil::stringToMono(morphShapes->getShape(i)->getName());
			scriptArray.set(i, monoString);
		}

		return scriptArray.getInternal();
	}
}