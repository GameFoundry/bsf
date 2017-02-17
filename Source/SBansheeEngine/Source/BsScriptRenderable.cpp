//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptRenderable.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsApplication.h"
#include "BsCRenderable.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsScriptMesh.h"
#include "BsScriptAnimation.h"
#include "BsScriptMaterial.h"
#include "BsScriptResourceManager.h"

namespace bs
{
	ScriptRenderable::ScriptRenderable(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mRenderable(nullptr)
	{
		mRenderable = Renderable::create();
		gSceneManager()._registerRenderable(mRenderable, parentSO);
	}

	ScriptRenderable::~ScriptRenderable()
	{ }

	void ScriptRenderable::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptRenderable::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_SetAnimation", &ScriptRenderable::internal_SetAnimation);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", &ScriptRenderable::internal_UpdateTransform);
		metaData.scriptClass->addInternalCall("Internal_SetMesh", &ScriptRenderable::internal_SetMesh);
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptRenderable::internal_GetBounds);
		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptRenderable::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptRenderable::internal_SetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetMaterial", &ScriptRenderable::internal_SetMaterial);
		metaData.scriptClass->addInternalCall("Internal_SetMaterials", &ScriptRenderable::internal_SetMaterials);
		metaData.scriptClass->addInternalCall("Internal_SetOverrideBounds", &ScriptRenderable::internal_SetOverrideBounds);
		metaData.scriptClass->addInternalCall("Internal_SetUseOverrideBounds", &ScriptRenderable::internal_SetUseOverrideBounds);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptRenderable::internal_OnDestroy);
	}

	void ScriptRenderable::updateTransform(const HSceneObject& parent, bool force)
	{
		mRenderable->_updateTransform(parent, force);

		if (parent->getActive() != mRenderable->getIsActive())
			mRenderable->setIsActive(parent->getActive());
	}

	void ScriptRenderable::internal_Create(MonoObject* instance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		new (bs_alloc<ScriptRenderable>()) ScriptRenderable(instance, so);
	}

	void ScriptRenderable::internal_SetAnimation(ScriptRenderable* thisPtr, ScriptAnimation* animation)
	{
		SPtr<Animation> anim;
		if (animation != nullptr)
			anim = animation->getInternal();

		thisPtr->getInternal()->setAnimation(anim);
	}

	void ScriptRenderable::internal_UpdateTransform(ScriptRenderable* thisPtr, ScriptSceneObject* parent, bool force)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		thisPtr->updateTransform(parentSO, force);
	}

	void ScriptRenderable::internal_SetMesh(ScriptRenderable* thisPtr, ScriptMesh* mesh)
	{
		HMesh nativeMesh;
		if (mesh != nullptr)
			nativeMesh = mesh->getHandle();

		thisPtr->getInternal()->setMesh(nativeMesh);
	}

	void ScriptRenderable::internal_GetBounds(ScriptRenderable* thisPtr, ScriptSceneObject* parent, AABox* box, Sphere* sphere)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();
		thisPtr->updateTransform(parentSO, false);

		Bounds bounds = thisPtr->getInternal()->getBounds();

		*box = bounds.getBox();
		*sphere = bounds.getSphere();
	}

	UINT64 ScriptRenderable::internal_GetLayers(ScriptRenderable* thisPtr)
	{
		return thisPtr->getInternal()->getLayer();
	}

	void ScriptRenderable::internal_SetLayers(ScriptRenderable* thisPtr, UINT64 layers)
	{
		thisPtr->getInternal()->setLayer(layers);
	}

	void ScriptRenderable::internal_SetMaterials(ScriptRenderable* thisPtr, MonoArray* materials)
	{
		if (materials != nullptr)
		{
			ScriptArray scriptMaterials(materials);

			Vector<HMaterial> nativeMaterials(scriptMaterials.size());
			for (UINT32 i = 0; i < scriptMaterials.size(); i++)
			{
				MonoObject* monoMaterial = scriptMaterials.get<MonoObject*>(i);
				ScriptMaterial* scriptMaterial = ScriptMaterial::toNative(monoMaterial);

				if (scriptMaterial != nullptr)
					nativeMaterials[i] = scriptMaterial->getHandle();
			}

			thisPtr->getInternal()->setMaterials(nativeMaterials);
		}
		else
		{
			thisPtr->getInternal()->setMaterials({});
		}
	}

	void ScriptRenderable::internal_SetMaterial(ScriptRenderable* thisPtr, ScriptMaterial* material, int index)
	{
		HMaterial nativeMaterial;
		if (material != nullptr)
			nativeMaterial = material->getHandle();

		thisPtr->getInternal()->setMaterial(index, nativeMaterial);
	}

	void ScriptRenderable::internal_SetOverrideBounds(ScriptRenderable* thisPtr, AABox* box)
	{
		thisPtr->getInternal()->setOverrideBounds(*box);
	}

	void ScriptRenderable::internal_SetUseOverrideBounds(ScriptRenderable* thisPtr, bool enable)
	{
		thisPtr->getInternal()->setUseOverrideBounds(enable);
	}

	void ScriptRenderable::internal_OnDestroy(ScriptRenderable* thisPtr)
	{
		thisPtr->destroy();
	}

	void ScriptRenderable::destroy()
	{
		if (mRenderable->isDestroyed())
			return;

		gSceneManager()._unregisterRenderable(mRenderable);
		mRenderable->destroy();
	}

	void ScriptRenderable::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}

	ScriptRenderable2::ScriptRenderable2(MonoObject* managedInstance, const HComponent& component)
		:TScriptComponent <ScriptRenderable2, Renderable>(managedInstance, component)
	{ }

	ScriptRenderable2::~ScriptRenderable2()
	{ }

	void ScriptRenderable2::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetMesh", &ScriptRenderable2::internal_GetMesh);
		metaData.scriptClass->addInternalCall("Internal_SetMesh", &ScriptRenderable2::internal_SetMesh);
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptRenderable2::internal_GetBounds);
		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptRenderable2::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptRenderable2::internal_SetLayers);
		metaData.scriptClass->addInternalCall("Internal_GetMaterial", &ScriptRenderable2::internal_GetMaterial);
		metaData.scriptClass->addInternalCall("Internal_SetMaterial", &ScriptRenderable2::internal_SetMaterial);
		metaData.scriptClass->addInternalCall("Internal_GetMaterials", &ScriptRenderable2::internal_GetMaterials);
		metaData.scriptClass->addInternalCall("Internal_SetMaterials", &ScriptRenderable2::internal_SetMaterials);
	}

	MonoObject* ScriptRenderable2::internal_GetMesh(ScriptRenderable2* thisPtr)
	{
		HMesh mesh = thisPtr->getHandle()->getMesh();

		if (!mesh.isLoaded(false))
			return nullptr;

		ScriptMesh* scriptMesh;
		ScriptResourceManager::instance().getScriptResource(mesh, &scriptMesh, true);

		return scriptMesh->getManagedInstance();
	}

	void ScriptRenderable2::internal_SetMesh(ScriptRenderable2* thisPtr, ScriptMesh* mesh)
	{
		HMesh nativeMesh;
		if (mesh != nullptr)
			nativeMesh = mesh->getHandle();

		thisPtr->getHandle()->setMesh(nativeMesh);
	}

	void ScriptRenderable2::internal_GetBounds(ScriptRenderable2* thisPtr, AABox* box, Sphere* sphere)
	{
		Bounds bounds = thisPtr->getHandle()->getBounds();

		*box = bounds.getBox();
		*sphere = bounds.getSphere();
	}

	UINT64 ScriptRenderable2::internal_GetLayers(ScriptRenderable2* thisPtr)
	{
		return thisPtr->getHandle()->getLayer();
	}

	void ScriptRenderable2::internal_SetLayers(ScriptRenderable2* thisPtr, UINT64 layers)
	{
		thisPtr->getHandle()->setLayer(layers);
	}

	MonoObject* ScriptRenderable2::internal_GetMaterial(ScriptRenderable2* thisPtr, int index)
	{
		HMaterial material = thisPtr->getHandle()->getMaterial(index);
		if (!material.isLoaded())
			return nullptr;

		ScriptMaterial* scriptMaterial;
		ScriptResourceManager::instance().getScriptResource(material, &scriptMaterial, true);

		return scriptMaterial->getManagedInstance();
	}

	void ScriptRenderable2::internal_SetMaterial(ScriptRenderable2* thisPtr, ScriptMaterial* material, int index)
	{
		HMaterial nativeMaterial;
		if (material != nullptr)
			nativeMaterial = material->getHandle();

		thisPtr->getHandle()->setMaterial(index, nativeMaterial);
	}

	MonoArray* ScriptRenderable2::internal_GetMaterials(ScriptRenderable2* thisPtr)
	{
		HRenderable renderable = thisPtr->getHandle();

		HMesh mesh = renderable->getMesh();
		if (!mesh.isLoaded())
			return ScriptArray::create<ScriptMaterial>(0).getInternal();

		UINT32 numMaterials = mesh->getProperties().getNumSubMeshes();
		ScriptArray output = ScriptArray::create<ScriptMaterial>(numMaterials);

		for(UINT32 i = 0; i < numMaterials; i++)
		{
			HMaterial material = renderable->getMaterial(i);
			if (!material.isLoaded())
				output.set(i, nullptr);
			else
			{
				ScriptMaterial* scriptMaterial;
				ScriptResourceManager::instance().getScriptResource(material, &scriptMaterial, true);

				output.set(i, scriptMaterial->getManagedInstance());
			}
		}

		return output.getInternal();
	}

	void ScriptRenderable2::internal_SetMaterials(ScriptRenderable2* thisPtr, MonoArray* materials)
	{
		if (materials != nullptr)
		{
			ScriptArray scriptMaterials(materials);

			Vector<HMaterial> nativeMaterials(scriptMaterials.size());
			for (UINT32 i = 0; i < scriptMaterials.size(); i++)
			{
				MonoObject* monoMaterial = scriptMaterials.get<MonoObject*>(i);
				ScriptMaterial* scriptMaterial = ScriptMaterial::toNative(monoMaterial);

				if (scriptMaterial != nullptr)
					nativeMaterials[i] = scriptMaterial->getHandle();
			}

			thisPtr->getHandle()->setMaterials(nativeMaterials);
		}
		else
		{
			thisPtr->getHandle()->setMaterials({});
		}
	}
}