#include "BsScriptRenderableHandler.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsApplication.h"
#include "BsRenderableHandler.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsScriptMesh.h"
#include "BsScriptMaterial.h"

namespace BansheeEngine
{
	ScriptRenderableHandler::ScriptRenderableHandler(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mRenderableHandler(nullptr), mLastUpdateHash(0)
	{
		mRenderableHandler = RenderableHandler::create();
		gSceneManager()._registerRenderable(mRenderableHandler, parentSO);
	}

	ScriptRenderableHandler::~ScriptRenderableHandler()
	{ }

	void ScriptRenderableHandler::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptRenderableHandler::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_UpdateTransform", &ScriptRenderableHandler::internal_UpdateTransform);
		metaData.scriptClass->addInternalCall("Internal_SetMesh", &ScriptRenderableHandler::internal_SetMesh);
		metaData.scriptClass->addInternalCall("Internal_GetBounds", &ScriptRenderableHandler::internal_GetBounds);
		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptRenderableHandler::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptRenderableHandler::internal_SetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetMaterial", &ScriptRenderableHandler::internal_SetMaterial);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptRenderableHandler::internal_OnDestroy);
	}

	void ScriptRenderableHandler::updateTransform(const HSceneObject& parent)
	{
		UINT32 curHash = parent->getTransformHash();
		if (curHash != mLastUpdateHash)
		{
			mRenderableHandler->setTransform(parent->getWorldTfrm());

			mLastUpdateHash = curHash;
		}

		if (parent->getActive() != mRenderableHandler->getIsActive())
		{
			mRenderableHandler->setIsActive(parent->getActive());
		}
	}

	void ScriptRenderableHandler::internal_Create(MonoObject* instance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		ScriptRenderableHandler* nativeInstance = new (bs_alloc<ScriptRenderableHandler>()) ScriptRenderableHandler(instance, so);
	}

	void ScriptRenderableHandler::internal_UpdateTransform(ScriptRenderableHandler* thisPtr, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		thisPtr->updateTransform(parentSO);
	}

	void ScriptRenderableHandler::internal_SetMesh(ScriptRenderableHandler* thisPtr, ScriptMesh* mesh)
	{
		HMesh nativeMesh;
		if (mesh != nullptr)
			nativeMesh = mesh->getMeshHandle();

		thisPtr->getInternal()->setMesh(nativeMesh);
	}

	void ScriptRenderableHandler::internal_GetBounds(ScriptRenderableHandler* thisPtr, ScriptSceneObject* parent, AABox* box, Sphere* sphere)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();
		thisPtr->updateTransform(parentSO);

		Bounds bounds = thisPtr->getInternal()->getBounds();

		*box = bounds.getBox();
		*sphere = bounds.getSphere();
	}

	UINT64 ScriptRenderableHandler::internal_GetLayers(ScriptRenderableHandler* thisPtr)
	{
		return thisPtr->getInternal()->getLayer();
	}

	void ScriptRenderableHandler::internal_SetLayers(ScriptRenderableHandler* thisPtr, UINT64 layers)
	{
		thisPtr->getInternal()->setLayer(layers);
	}

	void ScriptRenderableHandler::internal_SetMaterial(ScriptRenderableHandler* thisPtr, ScriptMaterial* material, int index)
	{
		HMaterial nativeMaterial;
		if (material != nullptr)
			nativeMaterial = material->getMaterialHandle();

		thisPtr->getInternal()->setMaterial(index, nativeMaterial);
	}

	void ScriptRenderableHandler::internal_OnDestroy(ScriptRenderableHandler* thisPtr)
	{
		thisPtr->destroy();
	}

	void ScriptRenderableHandler::destroy()
	{
		gSceneManager()._unregisterRenderable(mRenderableHandler);
		mRenderableHandler->destroy();
	}
}