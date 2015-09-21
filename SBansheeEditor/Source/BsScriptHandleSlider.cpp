#include "BsScriptHandleSlider.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsScriptHandleSliderManager.h"

namespace BansheeEngine
{
	ScriptHandleSliderBase::ScriptHandleSliderBase(MonoObject* managedInstance)
		:ScriptObjectBase(managedInstance)
	{
		ScriptHandleSliderManager::instance().registerSlider(this);
	}

	ScriptHandleSliderBase::~ScriptHandleSliderBase()
	{
		ScriptHandleSliderManager::instance().unregisterSlider(this);
	}

	void ScriptHandleSliderBase::destroy()
	{
		ScriptHandleSliderManager::instance().unregisterSlider(this);

		destroyInternal();
	}

	ScriptHandleSlider::ScriptHandleSlider(MonoObject* instance)
		: ScriptObject(instance)
	{

	}

	void ScriptHandleSlider::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptHandleSlider::internal_Destroy);
		metaData.scriptClass->addInternalCall("Internal_GetPosition", &ScriptHandleSlider::internal_GetPosition);
		metaData.scriptClass->addInternalCall("Internal_SetPosition", &ScriptHandleSlider::internal_SetPosition);
		metaData.scriptClass->addInternalCall("Internal_GetRotation", &ScriptHandleSlider::internal_GetRotation);
		metaData.scriptClass->addInternalCall("Internal_SetRotation", &ScriptHandleSlider::internal_SetRotation);
		metaData.scriptClass->addInternalCall("Internal_SetScale", &ScriptHandleSlider::internal_SetScale);
		metaData.scriptClass->addInternalCall("Internal_GetScale", &ScriptHandleSlider::internal_GetScale);
		metaData.scriptClass->addInternalCall("Internal_GetState", &ScriptHandleSlider::internal_GetState);
	}

	void ScriptHandleSlider::internal_Destroy(ScriptHandleSliderBase* nativeInstance)
	{
		nativeInstance->destroy();
	}

	void ScriptHandleSlider::internal_GetPosition(ScriptHandleSliderBase* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->getSlider()->getPosition();
	}

	void ScriptHandleSlider::internal_SetPosition(ScriptHandleSliderBase* nativeInstance, Vector3 value)
	{
		nativeInstance->getSlider()->setPosition(value);
	}

	void ScriptHandleSlider::internal_GetRotation(ScriptHandleSliderBase* nativeInstance, Quaternion* value)
	{
		*value = nativeInstance->getSlider()->getRotation();
	}

	void ScriptHandleSlider::internal_SetRotation(ScriptHandleSliderBase* nativeInstance, Quaternion value)
	{
		nativeInstance->getSlider()->setRotation(value);
	}

	void ScriptHandleSlider::internal_GetScale(ScriptHandleSliderBase* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->getSlider()->getScale();
	}

	void ScriptHandleSlider::internal_SetScale(ScriptHandleSliderBase* nativeInstance, Vector3 value)
	{
		nativeInstance->getSlider()->setScale(value);
	}

	void ScriptHandleSlider::internal_GetState(ScriptHandleSliderBase* nativeInstance, HandleSlider::State* value)
	{
		*value = nativeInstance->getSlider()->getState();
	}
}