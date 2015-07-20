#include "BsScriptLightInternal.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsScriptSceneObject.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	ScriptLightInternal::ScriptLightInternal(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mLightInternal(nullptr), mLastUpdateHash(0)
	{
		mLightInternal = LightInternal::create();
		gSceneManager()._registerLight(mLightInternal, parentSO);
	}

	ScriptLightInternal::~ScriptLightInternal()
	{

	}

	void ScriptLightInternal::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptLightInternal::internal_create);

		metaData.scriptClass->addInternalCall("Internal_GetPosition", &ScriptLightInternal::internal_getPosition);
		metaData.scriptClass->addInternalCall("Internal_SetPosition", &ScriptLightInternal::internal_setPosition);
		metaData.scriptClass->addInternalCall("Internal_GetRotation", &ScriptLightInternal::internal_getRotation);
		metaData.scriptClass->addInternalCall("Internal_SetRotation", &ScriptLightInternal::internal_setRotation);
		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptLightInternal::internal_getType);
		metaData.scriptClass->addInternalCall("Internal_SetType", &ScriptLightInternal::internal_setType);
		metaData.scriptClass->addInternalCall("Internal_GetCastsShadow", &ScriptLightInternal::internal_getCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_SetCastsShadow", &ScriptLightInternal::internal_setCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_GetColor", &ScriptLightInternal::internal_getColor);
		metaData.scriptClass->addInternalCall("Internal_SetColor", &ScriptLightInternal::internal_setColor);
		metaData.scriptClass->addInternalCall("Internal_GetRange", &ScriptLightInternal::internal_getRange);
		metaData.scriptClass->addInternalCall("Internal_SetRange", &ScriptLightInternal::internal_setRange);
		metaData.scriptClass->addInternalCall("Internal_GetIntensity", &ScriptLightInternal::internal_getIntensity);
		metaData.scriptClass->addInternalCall("Internal_SetIntensity", &ScriptLightInternal::internal_setIntensity);
		metaData.scriptClass->addInternalCall("Internal_GetSpotAngle", &ScriptLightInternal::internal_getSpotAngle);
		metaData.scriptClass->addInternalCall("Internal_SetSpotAngle", &ScriptLightInternal::internal_setSpotAngle);
		metaData.scriptClass->addInternalCall("Internal_GetSpotFalloffAngle", &ScriptLightInternal::internal_getSpotFalloffAngle);
		metaData.scriptClass->addInternalCall("Internal_SetSpotFalloffAngle", &ScriptLightInternal::internal_setSpotFalloffAngle);
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptLightInternal::internal_getBounds);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", &ScriptLightInternal::internal_updateTransform);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptLightInternal::internal_onDestroy);
	}

	void ScriptLightInternal::internal_create(MonoObject* managedInstance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		ScriptLightInternal* nativeInstance = new (bs_alloc<ScriptLightInternal>()) ScriptLightInternal(managedInstance, so);
	}

	Vector3 ScriptLightInternal::internal_getPosition(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getPosition();
	}

	void ScriptLightInternal::internal_setPosition(ScriptLightInternal* thisPtr, Vector3 position)
	{
		thisPtr->getInternal()->setPosition(position);
	}

	Quaternion ScriptLightInternal::internal_getRotation(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getRotation();
	}

	void ScriptLightInternal::internal_setRotation(ScriptLightInternal* thisPtr, Quaternion rotation)
	{
		thisPtr->getInternal()->setRotation(rotation);
	}

	LightType ScriptLightInternal::internal_getType(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getType();
	}

	void ScriptLightInternal::internal_setType(ScriptLightInternal* thisPtr, LightType type)
	{
		thisPtr->getInternal()->setType(type);
	}

	bool ScriptLightInternal::internal_getCastsShadow(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getCastsShadow();
	}

	void ScriptLightInternal::internal_setCastsShadow(ScriptLightInternal* thisPtr, bool castsShadow)
	{
		thisPtr->getInternal()->setCastsShadow(castsShadow);
	}

	Color ScriptLightInternal::internal_getColor(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getColor();
	}

	void ScriptLightInternal::internal_setColor(ScriptLightInternal* thisPtr, Color color)
	{
		thisPtr->getInternal()->setColor(color);
	}

	float ScriptLightInternal::internal_getRange(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getRange();
	}

	void ScriptLightInternal::internal_setRange(ScriptLightInternal* thisPtr, float range)
	{
		thisPtr->getInternal()->setRange(range);
	}

	float ScriptLightInternal::internal_getIntensity(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getIntensity();
	}

	void ScriptLightInternal::internal_setIntensity(ScriptLightInternal* thisPtr, float intensity)
	{
		thisPtr->getInternal()->setIntensity(intensity);
	}

	Degree ScriptLightInternal::internal_getSpotAngle(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getSpotAngle();
	}

	void ScriptLightInternal::internal_setSpotAngle(ScriptLightInternal* thisPtr, Degree spotAngle)
	{
		thisPtr->getInternal()->setSpotAngle(spotAngle);
	}

	Degree ScriptLightInternal::internal_getSpotFalloffAngle(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getSpotFalloffAngle();
	}

	void ScriptLightInternal::internal_setSpotFalloffAngle(ScriptLightInternal* thisPtr, Degree spotFalloffAngle)
	{
		thisPtr->getInternal()->setSpotFalloffAngle(spotFalloffAngle);
	}

	Sphere ScriptLightInternal::internal_getBounds(ScriptLightInternal* thisPtr)
	{
		return thisPtr->getInternal()->getBounds();
	}

	void ScriptLightInternal::internal_updateTransform(ScriptLightInternal* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		if (!parentSO.isDestroyed())
			thisPtr->getInternal()->_updateTransform(parentSO);
	}

	void ScriptLightInternal::internal_onDestroy(ScriptLightInternal* instance)
	{
		gSceneManager()._unregisterLight(instance->getInternal());
		instance->getInternal()->destroy();
	}
}