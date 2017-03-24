//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptLight.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"

namespace bs
{
	ScriptLight::ScriptLight(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mLight(nullptr), mLastUpdateHash(0)
	{
		mLight = Light::create();
		gSceneManager()._registerLight(mLight, parentSO);
	}

	ScriptLight::~ScriptLight()
	{ }

	void ScriptLight::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptLight::internal_create);

		metaData.scriptClass->addInternalCall("Internal_GetType", &ScriptLight::internal_getType);
		metaData.scriptClass->addInternalCall("Internal_SetType", &ScriptLight::internal_setType);
		metaData.scriptClass->addInternalCall("Internal_GetCastsShadow", &ScriptLight::internal_getCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_SetCastsShadow", &ScriptLight::internal_setCastsShadow);
		metaData.scriptClass->addInternalCall("Internal_GetUseAutoAttenuation", 
			&ScriptLight::internal_getUseAutoAttenuation);
		metaData.scriptClass->addInternalCall("Internal_SetUseAutoAttenuation", 
			&ScriptLight::internal_setUseAutoAttenuation);
		metaData.scriptClass->addInternalCall("Internal_GetColor", &ScriptLight::internal_getColor);
		metaData.scriptClass->addInternalCall("Internal_SetColor", &ScriptLight::internal_setColor);
		metaData.scriptClass->addInternalCall("Internal_GetAttenuationRadius", &ScriptLight::internal_getAttenuationRadius);
		metaData.scriptClass->addInternalCall("Internal_SetAttenuationRadius", &ScriptLight::internal_setAttenuationRadius);
		metaData.scriptClass->addInternalCall("Internal_GetSourceRadius", &ScriptLight::internal_getSourceRadius);
		metaData.scriptClass->addInternalCall("Internal_SetSourceRadius", &ScriptLight::internal_setSourceRadius);
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

		new (bs_alloc<ScriptLight>()) ScriptLight(managedInstance, so);
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

	bool ScriptLight::internal_getUseAutoAttenuation(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getUseAutoAttenuation();
	}

	void ScriptLight::internal_setUseAutoAttenuation(ScriptLight* thisPtr, bool value)
	{
		thisPtr->getInternal()->setUseAutoAttenuation(value);
	}

	void ScriptLight::internal_getColor(ScriptLight* thisPtr, Color* color)
	{
		*color = thisPtr->getInternal()->getColor();
	}

	void ScriptLight::internal_setColor(ScriptLight* thisPtr, Color color)
	{
		thisPtr->getInternal()->setColor(color);
	}

	float ScriptLight::internal_getAttenuationRadius(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getAttenuationRadius();
	}

	void ScriptLight::internal_setAttenuationRadius(ScriptLight* thisPtr, float radius)
	{
		thisPtr->getInternal()->setAttenuationRadius(radius);
	}

	float ScriptLight::internal_getSourceRadius(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getSourceRadius();
	}

	void ScriptLight::internal_setSourceRadius(ScriptLight* thisPtr, float radius)
	{
		thisPtr->getInternal()->setSourceRadius(radius);
	}

	float ScriptLight::internal_getIntensity(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getIntensity();
	}

	void ScriptLight::internal_setIntensity(ScriptLight* thisPtr, float intensity)
	{
		thisPtr->getInternal()->setIntensity(intensity);
	}

	float ScriptLight::internal_getSpotAngle(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getSpotAngle().valueDegrees();
	}

	void ScriptLight::internal_setSpotAngle(ScriptLight* thisPtr, float spotAngle)
	{
		thisPtr->getInternal()->setSpotAngle(Degree(spotAngle));
	}

	float ScriptLight::internal_getSpotFalloffAngle(ScriptLight* thisPtr)
	{
		return thisPtr->getInternal()->getSpotFalloffAngle().valueDegrees();
	}

	void ScriptLight::internal_setSpotFalloffAngle(ScriptLight* thisPtr, float spotFalloffAngle)
	{
		thisPtr->getInternal()->setSpotFalloffAngle(Degree(spotFalloffAngle));
	}

	void ScriptLight::internal_getBounds(ScriptLight* thisPtr, Sphere* bounds)
	{
		*bounds = thisPtr->getInternal()->getBounds();
	}

	void ScriptLight::internal_updateTransform(ScriptLight* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		if (!parentSO.isDestroyed())
		{
			thisPtr->getInternal()->_updateTransform(parentSO);

			if (parentSO->getActive() != thisPtr->getInternal()->getIsActive())
			{
				thisPtr->getInternal()->setIsActive(parentSO->getActive());
			}
		}
	}

	void ScriptLight::internal_onDestroy(ScriptLight* instance)
	{
		instance->destroy();
	}

	void ScriptLight::destroy()
	{
		if (mLight->isDestroyed())
			return;

		gSceneManager()._unregisterLight(mLight);
		mLight->destroy();
	}

	void ScriptLight::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}
}