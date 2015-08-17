#include "BsScriptMaterial.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptShader.h"
#include "BsScriptTexture2D.h"
#include "BsScriptTexture3D.h"
#include "BsScriptTextureCube.h"
#include <BsBuiltinResources.h>

namespace BansheeEngine
{
	ScriptMaterial::ScriptMaterial(MonoObject* instance, const HMaterial& material)
		:ScriptObject(instance), mMaterial(material)
	{

	}

	void ScriptMaterial::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptMaterial::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_Clone", &ScriptMaterial::internal_Clone);

		metaData.scriptClass->addInternalCall("Internal_GetShader", &ScriptMaterial::internal_GetShader);
		metaData.scriptClass->addInternalCall("Internal_SetShader", &ScriptMaterial::internal_SetShader);

		metaData.scriptClass->addInternalCall("Internal_SetFloat", &ScriptMaterial::internal_SetFloat);
		metaData.scriptClass->addInternalCall("Internal_SetVector2", &ScriptMaterial::internal_SetVector2);
		metaData.scriptClass->addInternalCall("Internal_SetVector3", &ScriptMaterial::internal_SetVector3);
		metaData.scriptClass->addInternalCall("Internal_SetVector4", &ScriptMaterial::internal_SetVector4);
		metaData.scriptClass->addInternalCall("Internal_SetMatrix3", &ScriptMaterial::internal_SetMatrix3);
		metaData.scriptClass->addInternalCall("Internal_SetMatrix4", &ScriptMaterial::internal_SetMatrix4);
		metaData.scriptClass->addInternalCall("Internal_SetColor", &ScriptMaterial::internal_SetColor);
		metaData.scriptClass->addInternalCall("Internal_SetTexture2D", &ScriptMaterial::internal_SetTexture2D);
		metaData.scriptClass->addInternalCall("Internal_SetTexture3D", &ScriptMaterial::internal_SetTexture3D);
		metaData.scriptClass->addInternalCall("Internal_SetTextureCube", &ScriptMaterial::internal_SetTextureCube);

		metaData.scriptClass->addInternalCall("Internal_GetFloat", &ScriptMaterial::internal_GetFloat);
		metaData.scriptClass->addInternalCall("Internal_GetVector2", &ScriptMaterial::internal_GetVector2);
		metaData.scriptClass->addInternalCall("Internal_GetVector3", &ScriptMaterial::internal_GetVector3);
		metaData.scriptClass->addInternalCall("Internal_GetVector4", &ScriptMaterial::internal_GetVector4);
		metaData.scriptClass->addInternalCall("Internal_GetMatrix3", &ScriptMaterial::internal_GetMatrix3);
		metaData.scriptClass->addInternalCall("Internal_GetMatrix4", &ScriptMaterial::internal_GetMatrix4);
		metaData.scriptClass->addInternalCall("Internal_GetColor", &ScriptMaterial::internal_GetColor);
		metaData.scriptClass->addInternalCall("Internal_GetTexture2D", &ScriptMaterial::internal_GetTexture2D);
		metaData.scriptClass->addInternalCall("Internal_GetTexture3D", &ScriptMaterial::internal_GetTexture3D);
		metaData.scriptClass->addInternalCall("Internal_GetTextureCube", &ScriptMaterial::internal_GetTextureCube);
	}

	void ScriptMaterial::internal_CreateInstance(MonoObject* instance, ScriptShader* shader)
	{
		HShader nativeShader;
		if (shader != nullptr)
			nativeShader = shader->getShaderHandle();

		if (nativeShader == nullptr)
			nativeShader = BuiltinResources::instance().getDummyShader();

		HMaterial material = Material::create(nativeShader);

		ScriptMaterial* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, material, &scriptInstance);
	}

	MonoObject* ScriptMaterial::internal_Clone(ScriptMaterial* nativeInstance)
	{
		HMaterial clone = nativeInstance->mMaterial->clone();

		ScriptMaterial* scriptClone;
		ScriptResourceManager::instance().createScriptResource(clone, &scriptClone);

		return scriptClone->getManagedInstance();
	}

	MonoObject* ScriptMaterial::internal_GetShader(ScriptMaterial* nativeInstance)
	{
		HShader shader = nativeInstance->getMaterialHandle()->getShader();

		if (shader == nullptr)
			return nullptr;

		ScriptShader* scriptShader;
		ScriptResourceManager::instance().getScriptResource(shader, &scriptShader, true);

		return scriptShader->getManagedInstance();
	}

	void ScriptMaterial::internal_SetShader(ScriptMaterial* nativeInstance, ScriptShader* shader)
	{
		HShader nativeShader;
		if (shader != nullptr)
			nativeShader = shader->getShaderHandle();

		if (nativeShader == nullptr)
			nativeShader = BuiltinResources::instance().getDummyShader();

		nativeInstance->getMaterialHandle()->setShader(nativeShader);
	}

	void ScriptMaterial::internal_SetFloat(ScriptMaterial* nativeInstance, MonoString* name, float value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setFloat(paramName, value);
	}

	void ScriptMaterial::internal_SetVector2(ScriptMaterial* nativeInstance, MonoString* name, Vector2 value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setVec2(paramName, value);
	}

	void ScriptMaterial::internal_SetVector3(ScriptMaterial* nativeInstance, MonoString* name, Vector3 value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setVec3(paramName, value);
	}

	void ScriptMaterial::internal_SetVector4(ScriptMaterial* nativeInstance, MonoString* name, Vector4 value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setVec4(paramName, value);
	}

	void ScriptMaterial::internal_SetMatrix3(ScriptMaterial* nativeInstance, MonoString* name, Matrix3 value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setMat3(paramName, value);
	}

	void ScriptMaterial::internal_SetMatrix4(ScriptMaterial* nativeInstance, MonoString* name, Matrix4 value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setMat4(paramName, value);
	}

	void ScriptMaterial::internal_SetColor(ScriptMaterial* nativeInstance, MonoString* name, Color value)
	{
		String paramName = MonoUtil::monoToString(name);

		nativeInstance->mMaterial->setColor(paramName, value);
	}

	void ScriptMaterial::internal_SetTexture2D(ScriptMaterial* nativeInstance, MonoString* name, ScriptTexture2D* value)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture;

		if (value != nullptr)
			texture = value->getTextureHandle();

		nativeInstance->mMaterial->setTexture(paramName, texture);
	}

	void ScriptMaterial::internal_SetTexture3D(ScriptMaterial* nativeInstance, MonoString* name, ScriptTexture3D* value)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture;

		if (value != nullptr)
			texture = value->getTextureHandle();

		nativeInstance->mMaterial->setTexture(paramName, texture);
	}

	void ScriptMaterial::internal_SetTextureCube(ScriptMaterial* nativeInstance, MonoString* name, ScriptTextureCube* value)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture;

		if (value != nullptr)
			texture = value->getTextureHandle();

		nativeInstance->mMaterial->setTexture(paramName, texture);
	}

	float ScriptMaterial::internal_GetFloat(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getFloat(paramName);
	}

	Vector2 ScriptMaterial::internal_GetVector2(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getVec2(paramName);
	}

	Vector3 ScriptMaterial::internal_GetVector3(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getVec3(paramName);
	}

	Vector4 ScriptMaterial::internal_GetVector4(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getVec4(paramName);
	}

	Matrix3 ScriptMaterial::internal_GetMatrix3(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getMat3(paramName);
	}

	Matrix4 ScriptMaterial::internal_GetMatrix4(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getMat4(paramName);
	}

	Color ScriptMaterial::internal_GetColor(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		return nativeInstance->mMaterial->getColor(paramName);
	}

	MonoObject* ScriptMaterial::internal_GetTexture2D(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture = nativeInstance->mMaterial->getTexture(paramName);
		if (texture == nullptr)
			return nullptr;

		ScriptTexture2D* scriptTexture;
		ScriptResourceManager::instance().getScriptResource(texture, &scriptTexture, true);

		return scriptTexture->getManagedInstance();
	}

	MonoObject* ScriptMaterial::internal_GetTexture3D(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture = nativeInstance->mMaterial->getTexture(paramName);
		if (texture == nullptr)
			return nullptr;

		ScriptTexture3D* scriptTexture;
		ScriptResourceManager::instance().getScriptResource(texture, &scriptTexture, true);

		return scriptTexture->getManagedInstance();
	}

	MonoObject* ScriptMaterial::internal_GetTextureCube(ScriptMaterial* nativeInstance, MonoString* name)
	{
		String paramName = MonoUtil::monoToString(name);

		HTexture texture = nativeInstance->mMaterial->getTexture(paramName);
		if (texture == nullptr)
			return nullptr;

		ScriptTextureCube* scriptTexture;
		ScriptResourceManager::instance().getScriptResource(texture, &scriptTexture, true);

		return scriptTexture->getManagedInstance();
	}

	void ScriptMaterial::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptMaterial::setNativeHandle(const HResource& resource)
	{
		mMaterial = static_resource_cast<Material>(resource);
	}
}