#include "BsScriptLight.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsScriptSceneObject.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	ScriptLight::ScriptLight(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mLight(nullptr), mLastUpdateHash(0)
	{
		mLight = Light::create();
		gSceneManager()._registerLight(mLight, parentSO);
	}

	ScriptLight::~ScriptLight()
	{

	}

	void ScriptLight::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptLight::internal_create);

		metaData.scriptClass->addInternalCall("Internal_GetPosition", &ScriptLight::internal_getPosition);
		metaData.scriptClass->addInternalCall("Internal_SetPosition", &ScriptLight::internal_setPosition);
		metaData.scriptClass->addInternalCall("Internal_GetRotation", &ScriptLight::internal_getRotation);
		metaData.scriptClass->addInternalCall("Internal_SetRotation", &ScriptLight::internal_setRotation);
		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptLight::internal_getType);
		metaData.scriptClass->addInternalCall("Internal_SetType", &ScriptLight::internal_setType);
		metaData.scriptClass->addInternalCall("Internal_GetCastsShadow", &ScriptLight::internal_getCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_SetCastsShadow", &ScriptLight::internal_setCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_GetColor", &ScriptLight::internal_getColor);
		metaData.scriptClass->addInternalCall("Internal_SetColor", &ScriptLight::internal_setColor);
		metaData.scriptClass->addInternalCall("Internal_GetRange", &ScriptLight::internal_getRange);
		metaData.scriptClass->addInternalCall("Internal_SetRange", &ScriptLight::internal_setRange);
		metaData.scriptClass->addInternalCall("Internal_GetIntensity", &ScriptLight::internal_getIntensity);
		metaData.scriptClass->addInternalCall("Internal_SetIntensity", &ScriptLight::internal_setIntensity);
		metaData.scriptClass->addInternalCall("Internal_GetSpotAngle", &ScriptLight::internal_getSpotAngle);
		metaData.scriptClass->addInternalCall("Internal_SetSpotAngle", &ScriptLight::internal_setSpotAngle);
		metaData.scriptClass->addInternalCall("Internal_GetSpotFalloffAngle", &ScriptLight::internal_getSpotFalloffAngle);
		metaData.scriptClass->addInternalCall("Internal_SetSpotFalloffAngle", &ScriptLight::internal_setSpotFalloffAngle);
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptLight::internal_getBounds);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", &ScriptLight::internal_updateTransform);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptLight::internal_onDestroy);
	}

	void ScriptLight::internal_create(MonoObject* managedInstance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		ScriptLight* nativeInstance = new (bs_alloc<ScriptLight>()) ScriptLight(managedInstance, so);
	}

	Vector3 ScriptLight::internal_getPosition(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getPosition();
	}

	void ScriptLight::internal_setPosition(ScriptLight* thisPtr, Vector3 position)
	{
		thisPtr->getInternal()->setPosition(position);
	}

	Quaternion ScriptLight::internal_getRotation(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getRotation();
	}

	void ScriptLight::internal_setRotation(ScriptLight* thisPtr, Quaternion rotation)
	{
		thisPtr->getInternal()->setRotation(rotation);
	}

	LightType ScriptLight::internal_getType(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getType();
	}

	void ScriptLight::internal_setType(ScriptLight* thisPtr, LightType type)
	{
		thisPtr->getInternal()->setType(type);
	}

	bool ScriptLight::internal_getCastsShadow(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getCastsShadow();
	}

	void ScriptLight::internal_setCastsShadow(ScriptLight* thisPtr, bool castsShadow)
	{
		thisPtr->getInternal()->setCastsShadow(castsShadow);
	}

	Color ScriptLight::internal_getColor(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getColor();
	}

	void ScriptLight::internal_setColor(ScriptLight* thisPtr, Color color)
	{
		thisPtr->getInternal()->setColor(color);
	}

	float ScriptLight::internal_getRange(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getRange();
	}

	void ScriptLight::internal_setRange(ScriptLight* thisPtr, float range)
	{
		thisPtr->getInternal()->setRange(range);
	}

	float ScriptLight::internal_getIntensity(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getIntensity();
	}

	void ScriptLight::internal_setIntensity(ScriptLight* thisPtr, float intensity)
	{
		thisPtr->getInternal()->setIntensity(intensity);
	}

	Degree ScriptLight::internal_getSpotAngle(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getSpotAngle();
	}

	void ScriptLight::internal_setSpotAngle(ScriptLight* thisPtr, Degree spotAngle)
	{
		thisPtr->getInternal()->setSpotAngle(spotAngle);
	}

	Degree ScriptLight::internal_getSpotFalloffAngle(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getSpotFalloffAngle();
	}

	void ScriptLight::internal_setSpotFalloffAngle(ScriptLight* thisPtr, Degree spotFalloffAngle)
	{
		thisPtr->getInternal()->setSpotFalloffAngle(spotFalloffAngle);
	}

	Sphere ScriptLight::internal_getBounds(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getBounds();
	}

	void ScriptLight::internal_updateTransform(ScriptLight* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		if (!parentSO.isDestroyed())
			thisPtr->getInternal()->_updateTransform(parentSO);
	}

	void ScriptLight::internal_onDestroy(ScriptLight* instance)
	{
		gSceneManager()._unregisterLight(instance->getInternal());
		instance->getInternal()->destroy();
	}
}