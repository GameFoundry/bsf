#include "BsScriptUnitTests.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsSceneObject.h"
#include "BsScriptSceneObject.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableDiff.h"

namespace BansheeEngine
{
	MonoMethod* ScriptUnitTests::RunTestsMethod;
	SPtr<ManagedSerializableDiff> ScriptUnitTests::tempDiff;

	void ScriptUnitTests::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_UT1_GameObjectClone", &ScriptUnitTests::internal_UT1_GameObjectClone);
		metaData.scriptClass->addInternalCall("Internal_UT3_GenerateDiff", &ScriptUnitTests::internal_UT3_GenerateDiff);
		metaData.scriptClass->addInternalCall("Internal_UT3_ApplyDiff", &ScriptUnitTests::internal_UT3_ApplyDiff);

		RunTestsMethod = metaData.scriptClass->getMethod("RunTests");
	}

	void ScriptUnitTests::runTests()
	{
		RunTestsMethod->invoke(nullptr, nullptr);
	}

	void ScriptUnitTests::internal_UT1_GameObjectClone(MonoObject* instance)
	{
		ScriptSceneObject* nativeInstance = ScriptSceneObject::toNative(instance);

		HSceneObject SO = static_object_cast<SceneObject>(nativeInstance->getNativeHandle());
		HSceneObject cloneSO = SO->clone();

		cloneSO->setParent(SO);
	}

	void ScriptUnitTests::internal_UT3_GenerateDiff(MonoObject* oldObj, MonoObject* newObj)
	{
		ManagedSerializableObjectPtr serializableOldObj = ManagedSerializableObject::createFromExisting(oldObj);
		ManagedSerializableObjectPtr serializableNewObj = ManagedSerializableObject::createFromExisting(newObj);

		tempDiff = ManagedSerializableDiff::create(serializableOldObj, serializableNewObj);
	}

	void ScriptUnitTests::internal_UT3_ApplyDiff(MonoObject* obj)
	{
		ManagedSerializableObjectPtr serializableObj = ManagedSerializableObject::createFromExisting(obj);
		tempDiff->apply(serializableObj);

		tempDiff = nullptr;
	}
}