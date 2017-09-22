//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptCamera.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "Renderer/BsCamera.h"
#include "Wrappers/BsScriptSceneObject.h"
#include "Scene/BsSceneObject.h"
#include "Wrappers/BsScriptRenderTarget.h"
#include "Scene/BsSceneManager.h"
#include "RenderAPI/BsViewport.h"
#include "RenderAPI/BsRenderTarget.h"
#include "RenderAPI/BsRenderWindow.h"
#include "RenderAPI/BsRenderTexture.h"
#include "GUI/BsGUIManager.h"
#include "Renderer/BsRenderSettings.h"
#include "Wrappers/BsScriptRenderSettings.h"

namespace bs
{
	ScriptCamera::ScriptCamera(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mCamera(nullptr), mLastUpdateHash(0)
	{ 
		mCamera = Camera::create(nullptr);
		gSceneManager()._registerCamera(mCamera, parentSO);
	}

	ScriptCamera::~ScriptCamera()
	{

	}

	void ScriptCamera::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", (void*)&ScriptCamera::internal_Create);

		metaData.scriptClass->addInternalCall("Internal_GetAspect", (void*)&ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetAspect", (void*)&ScriptCamera::internal_SetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetNearClip", (void*)&ScriptCamera::internal_GetNearClip);
		metaData.scriptClass->addInternalCall("Internal_SetNearClip", (void*)&ScriptCamera::internal_SetNearClip);

		metaData.scriptClass->addInternalCall("Internal_GetFarClip", (void*)&ScriptCamera::internal_GetFarClip);
		metaData.scriptClass->addInternalCall("Internal_SetFarClip", (void*)&ScriptCamera::internal_SetFarClip);

		metaData.scriptClass->addInternalCall("Internal_GetFieldOfView", (void*)&ScriptCamera::internal_GetFieldOfView);
		metaData.scriptClass->addInternalCall("Internal_SetFieldOfView", (void*)&ScriptCamera::internal_SetFieldOfView);

		metaData.scriptClass->addInternalCall("Internal_GetViewportRect", (void*)&ScriptCamera::internal_GetViewportRect);
		metaData.scriptClass->addInternalCall("Internal_SetViewportRect", (void*)&ScriptCamera::internal_SetViewportRect);

		metaData.scriptClass->addInternalCall("Internal_GetProjectionType", (void*)&ScriptCamera::internal_GetProjectionType);
		metaData.scriptClass->addInternalCall("Internal_SetProjectionType", (void*)&ScriptCamera::internal_SetProjectionType);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicHeight", (void*)&ScriptCamera::internal_GetOrthographicHeight);
		metaData.scriptClass->addInternalCall("Internal_SetOrthographicHeight", (void*)&ScriptCamera::internal_SetOrthographicHeight);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicWidth", (void*)&ScriptCamera::internal_GetOrthographicWidth);

		metaData.scriptClass->addInternalCall("Internal_GetClearColor", (void*)&ScriptCamera::internal_GetClearColor);
		metaData.scriptClass->addInternalCall("Internal_SetClearColor", (void*)&ScriptCamera::internal_SetClearColor);

		metaData.scriptClass->addInternalCall("Internal_GetDepthClearValue", (void*)&ScriptCamera::internal_GetDepthClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetDepthClearValue", (void*)&ScriptCamera::internal_SetDepthClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetStencilClearValue", (void*)&ScriptCamera::internal_GetStencilClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetStencilClearValue", (void*)&ScriptCamera::internal_SetStencilClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetClearFlags", (void*)&ScriptCamera::internal_GetClearFlags);
		metaData.scriptClass->addInternalCall("Internal_SetClearFlags", (void*)&ScriptCamera::internal_SetClearFlags);

		metaData.scriptClass->addInternalCall("Internal_GetPriority", (void*)&ScriptCamera::internal_GetPriority);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", (void*)&ScriptCamera::internal_SetPriority);

		metaData.scriptClass->addInternalCall("Internal_GetRenderSettings", (void*)&ScriptCamera::internal_GetRenderSettings);
		metaData.scriptClass->addInternalCall("Internal_SetRenderSettings", (void*)&ScriptCamera::internal_SetRenderSettings);

		metaData.scriptClass->addInternalCall("Internal_GetLayers", (void*)&ScriptCamera::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", (void*)&ScriptCamera::internal_SetLayers);

		metaData.scriptClass->addInternalCall("Internal_GetProjMatrix", (void*)&ScriptCamera::internal_GetProjMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetProjMatrixInv", (void*)&ScriptCamera::internal_GetProjMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetViewMatrix", (void*)&ScriptCamera::internal_GetViewMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetViewMatrixInv", (void*)&ScriptCamera::internal_GetViewMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetWidthPixels", (void*)&ScriptCamera::internal_GetWidthPixels);
		metaData.scriptClass->addInternalCall("Internal_GetHeightPixels", (void*)&ScriptCamera::internal_GetHeightPixels);

		metaData.scriptClass->addInternalCall("Internal_WorldToViewport", (void*)&ScriptCamera::internal_WorldToViewport);
		metaData.scriptClass->addInternalCall("Internal_WorldToNDC", (void*)&ScriptCamera::internal_WorldToNDC);
		metaData.scriptClass->addInternalCall("Internal_WorldToView", (void*)&ScriptCamera::internal_WorldToView);

		metaData.scriptClass->addInternalCall("Internal_ScreenToViewport", (void*)&ScriptCamera::internal_ScreenToViewport);

		metaData.scriptClass->addInternalCall("Internal_ViewportToWorld", (void*)&ScriptCamera::internal_ViewportToWorld);
		metaData.scriptClass->addInternalCall("Internal_ViewportToView", (void*)&ScriptCamera::internal_ViewportToView);
		metaData.scriptClass->addInternalCall("Internal_ViewportToNDC", (void*)&ScriptCamera::internal_ViewportToNDC);

		metaData.scriptClass->addInternalCall("Internal_ViewToWorld", (void*)&ScriptCamera::internal_ViewToWorld);
		metaData.scriptClass->addInternalCall("Internal_ViewToViewport", (void*)&ScriptCamera::internal_ViewToViewport);
		metaData.scriptClass->addInternalCall("Internal_ViewToNDC", (void*)&ScriptCamera::internal_ViewToNDC);

		metaData.scriptClass->addInternalCall("Internal_NDCToWorld", (void*)&ScriptCamera::internal_NDCToWorld);
		metaData.scriptClass->addInternalCall("Internal_NDCToView", (void*)&ScriptCamera::internal_NDCToView);
		metaData.scriptClass->addInternalCall("Internal_NDCToViewport", (void*)&ScriptCamera::internal_NDCToViewport);

		metaData.scriptClass->addInternalCall("Internal_ViewportToWorldRay", (void*)&ScriptCamera::internal_ViewportToWorldRay);
		metaData.scriptClass->addInternalCall("Internal_ScreenToWorldRay", (void*)&ScriptCamera::internal_ScreenToWorldRay);

		metaData.scriptClass->addInternalCall("Internal_ProjectPoint", (void*)&ScriptCamera::internal_ProjectPoint);
		metaData.scriptClass->addInternalCall("Internal_UnprojectPoint", (void*)&ScriptCamera::internal_UnprojectPoint);

		metaData.scriptClass->addInternalCall("Internal_SetRenderTarget", (void*)&ScriptCamera::internal_SetRenderTarget);

		metaData.scriptClass->addInternalCall("Internal_GetMain", (void*)&ScriptCamera::internal_GetMain);
		metaData.scriptClass->addInternalCall("Internal_SetMain", (void*)&ScriptCamera::internal_SetMain);

		metaData.scriptClass->addInternalCall("Internal_UpdateView", (void*)&ScriptCamera::internal_UpdateView);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", (void*)&ScriptCamera::internal_OnDestroy);
	}

	void ScriptCamera::updateView(const HSceneObject& parent)
	{
		if (parent.isDestroyed())
			return;

		UINT32 curHash = parent->getTransformHash();
		if (curHash != mLastUpdateHash)
		{
			mCamera->setPosition(parent->getWorldPosition());
			mCamera->setRotation(parent->getWorldRotation());

			mLastUpdateHash = curHash;
		}

		if (parent->getActive() != mCamera->getIsActive())
		{
			mCamera->setIsActive(parent->getActive());
		}
	}

	void ScriptCamera::internal_Create(MonoObject* managedInstance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		new (bs_alloc<ScriptCamera>()) ScriptCamera(managedInstance, so);
	}

	float ScriptCamera::internal_GetAspect(ScriptCamera* instance)
	{
		return instance->mCamera->getAspectRatio();
	}

	void ScriptCamera::internal_SetAspect(ScriptCamera* instance, float value)
	{
		instance->mCamera->setAspectRatio(value);
	}

	float ScriptCamera::internal_GetNearClip(ScriptCamera* instance)
	{
		return instance->mCamera->getNearClipDistance();
	}

	void ScriptCamera::internal_SetNearClip(ScriptCamera* instance, float value)
	{
		instance->mCamera->setNearClipDistance(value);
	}

	float ScriptCamera::internal_GetFarClip(ScriptCamera* instance)
	{
		return instance->mCamera->getFarClipDistance();
	}

	void ScriptCamera::internal_SetFarClip(ScriptCamera* instance, float value)
	{
		instance->mCamera->setFarClipDistance(value);
	}

	void ScriptCamera::internal_GetFieldOfView(ScriptCamera* instance, Degree* value)
	{
		*value = instance->mCamera->getHorzFOV();
	}

	void ScriptCamera::internal_SetFieldOfView(ScriptCamera* instance, Degree* value)
	{
		instance->mCamera->setHorzFOV(*value);
	}

	void ScriptCamera::internal_GetViewportRect(ScriptCamera* instance, Rect2* value)
	{
		*value = instance->mCamera->getViewport()->getNormArea();
	}

	void ScriptCamera::internal_SetViewportRect(ScriptCamera* instance, Rect2* value)
	{
		instance->mCamera->getViewport()->setArea(value->x, value->y, value->width, value->height);
	}

	UINT32 ScriptCamera::internal_GetProjectionType(ScriptCamera* instance)
	{
		return instance->mCamera->getProjectionType();
	}

	void ScriptCamera::internal_SetProjectionType(ScriptCamera* instance, UINT32 value)
	{
		instance->mCamera->setProjectionType((ProjectionType)value);
	}

	float ScriptCamera::internal_GetOrthographicHeight(ScriptCamera* instance)
	{
		return instance->mCamera->getOrthoWindowHeight();
	}

	void ScriptCamera::internal_SetOrthographicHeight(ScriptCamera* instance, float value)
	{
		instance->mCamera->setOrthoWindowHeight(value);
	}

	float ScriptCamera::internal_GetOrthographicWidth(ScriptCamera* instance)
	{
		return instance->mCamera->getOrthoWindowWidth();
	}

	void ScriptCamera::internal_GetClearColor(ScriptCamera* instance, Color* value)
	{
		*value = instance->mCamera->getViewport()->getClearColor();
	}

	void ScriptCamera::internal_SetClearColor(ScriptCamera* instance, Color* value)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();
		vp->setClearValues(*value, vp->getClearDepthValue(), vp->getClearStencilValue());
	}

	float ScriptCamera::internal_GetDepthClearValue(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getClearDepthValue();
	}

	void ScriptCamera::internal_SetDepthClearValue(ScriptCamera* instance, float value)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();
		vp->setClearValues(vp->getClearColor(), value, vp->getClearStencilValue());
	}

	UINT16 ScriptCamera::internal_GetStencilClearValue(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getClearStencilValue();
	}

	void ScriptCamera::internal_SetStencilClearValue(ScriptCamera* instance, UINT16 value)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();
		vp->setClearValues(vp->getClearColor(), vp->getClearDepthValue(), value);
	}

	UINT32 ScriptCamera::internal_GetClearFlags(ScriptCamera* instance)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();
		UINT32 clearFlags = 0;

		clearFlags |= vp->getRequiresColorClear() ? 0x01 : 0;
		clearFlags |= vp->getRequiresDepthClear() ? 0x02 : 0;
		clearFlags |= vp->getRequiresStencilClear() ? 0x04 : 0;

		return clearFlags;
	}

	void ScriptCamera::internal_SetClearFlags(ScriptCamera* instance, UINT32 value)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();

		vp->setRequiresClear((value & 0x01) != 0,
			(value & 0x02) != 0, (value & 0x04) != 0);
	}

	int ScriptCamera::internal_GetPriority(ScriptCamera* instance)
	{
		return instance->mCamera->getPriority();
	}

	void ScriptCamera::internal_SetPriority(ScriptCamera* instance, int value)
	{
		instance->mCamera->setPriority(value);
	}

	MonoObject* ScriptCamera::internal_GetRenderSettings(ScriptCamera* instance)
	{
		SPtr<RenderSettings> settings = instance->mCamera->getRenderSettings();
		if (settings == nullptr)
			settings = bs_shared_ptr_new<RenderSettings>();

		return ScriptRenderSettings::toManaged(settings);
	}

	void ScriptCamera::internal_SetRenderSettings(ScriptCamera* instance, MonoObject* value)
	{
		instance->mCamera->setRenderSettings(ScriptRenderSettings::toNative(value));
	}

	UINT64 ScriptCamera::internal_GetLayers(ScriptCamera* instance)
	{
		return instance->mCamera->getLayers();
	}

	void ScriptCamera::internal_SetLayers(ScriptCamera* instance, UINT64 value)
	{
		instance->mCamera->setLayers(value);
	}

	void ScriptCamera::internal_GetProjMatrix(ScriptCamera* instance, Matrix4* output)
	{
		*output = instance->mCamera->getProjectionMatrixRS();
	}

	void ScriptCamera::internal_GetProjMatrixInv(ScriptCamera* instance, Matrix4* output)
	{
		*output = instance->mCamera->getProjectionMatrixRSInv();
	}

	void ScriptCamera::internal_GetViewMatrix(ScriptCamera* instance, Matrix4* output)
	{
		*output = instance->mCamera->getViewMatrix();
	}

	void ScriptCamera::internal_GetViewMatrixInv(ScriptCamera* instance, Matrix4* output)
	{
		*output = instance->mCamera->getViewMatrixInv();
	}

	int ScriptCamera::internal_GetWidthPixels(ScriptCamera* instance)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();

		return vp->getWidth();
	}

	int ScriptCamera::internal_GetHeightPixels(ScriptCamera* instance)
	{
		SPtr<Viewport> vp = instance->mCamera->getViewport();

		return vp->getHeight();
	}

	void ScriptCamera::internal_WorldToViewport(ScriptCamera* instance, Vector3* value, Vector2I* output)
	{
		*output = instance->mCamera->worldToScreenPoint(*value);
	}

	void ScriptCamera::internal_WorldToNDC(ScriptCamera* instance, Vector3* value, Vector2* output)
	{
		*output = instance->mCamera->worldToNdcPoint(*value);
	}

	void ScriptCamera::internal_WorldToView(ScriptCamera* instance, Vector3* value, Vector3* output)
	{
		*output = instance->mCamera->worldToViewPoint(*value);
	}

	void ScriptCamera::internal_ScreenToViewport(ScriptCamera* instance, Vector2I* value, Vector2I* output)
	{
		SceneCameraData mainCamera = gSceneManager().getMainCamera();
		if (mainCamera.camera == nullptr)
			return;

		// The main camera could be rendering to a standalone window, or be a part of the editor GUI. Find out which
		// and transform the pointer position appropriately.
		SPtr<RenderTarget> target = mainCamera.camera->getViewport()->getTarget();
		if (target == nullptr)
			return;

		if (target->getProperties().isWindow)
		{
			SPtr<RenderWindow> window = std::static_pointer_cast<RenderWindow>(target);
			*output = window->screenToWindowPos(*value);
		}
		else
		{
			SPtr<RenderTexture> texture = std::static_pointer_cast<RenderTexture>(target);

			SPtr<RenderWindow> window = GUIManager::instance().getBridgeWindow(texture);
			if (window == nullptr)
				return;

			*output = window->screenToWindowPos(*value);
			*output = GUIManager::instance().windowToBridgedCoords(target, *output);
		}
	}

	void ScriptCamera::internal_ViewportToWorld(ScriptCamera* instance, Vector2I* value, float depth, Vector3* output)
	{
		*output = instance->mCamera->screenToWorldPoint(*value, depth);
	}

	void ScriptCamera::internal_ViewportToView(ScriptCamera* instance, Vector2I* value, float depth, Vector3* output)
	{
		*output = instance->mCamera->screenToViewPoint(*value, depth);
	}

	void ScriptCamera::internal_ViewportToNDC(ScriptCamera* instance, Vector2I* value, Vector2* output)
	{
		*output = instance->mCamera->screenToNdcPoint(*value);
	}

	void ScriptCamera::internal_ViewToWorld(ScriptCamera* instance, Vector3* value, Vector3* output)
	{
		*output = instance->mCamera->viewToWorldPoint(*value);
	}

	void ScriptCamera::internal_ViewToViewport(ScriptCamera* instance, Vector3* value, Vector2I* output)
	{
		*output = instance->mCamera->viewToScreenPoint(*value);
	}

	void ScriptCamera::internal_ViewToNDC(ScriptCamera* instance, Vector3* value, Vector2* output)
	{
		*output = instance->mCamera->viewToNdcPoint(*value);
	}

	void ScriptCamera::internal_NDCToWorld(ScriptCamera* instance, Vector2* value, float depth, Vector3* output)
	{
		*output = instance->mCamera->ndcToWorldPoint(*value, depth);
	}

	void ScriptCamera::internal_NDCToView(ScriptCamera* instance, Vector2* value, float depth, Vector3* output)
	{
		*output = instance->mCamera->ndcToViewPoint(*value, depth);
	}

	void ScriptCamera::internal_NDCToViewport(ScriptCamera* instance, Vector2* value, Vector2I* output)
	{
		*output = instance->mCamera->ndcToScreenPoint(*value);
	}

	void ScriptCamera::internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I* value, Ray* output)
	{
		Vector2I viewportPos;
		internal_ScreenToViewport(instance, value, &viewportPos);
		internal_ViewportToWorldRay(instance, &viewportPos, output);
	}

	void ScriptCamera::internal_ViewportToWorldRay(ScriptCamera* instance, Vector2I* value, Ray* output)
	{
		*output = instance->mCamera->screenPointToRay(*value);
	}

	void ScriptCamera::internal_ProjectPoint(ScriptCamera* instance, Vector3* value, Vector3* output)
	{
		*output = instance->mCamera->projectPoint(*value);
	}

	void ScriptCamera::internal_UnprojectPoint(ScriptCamera* instance, Vector3* value, Vector3* output)
	{
		*output = instance->mCamera->unprojectPoint(*value);
	}

	void ScriptCamera::internal_SetRenderTarget(ScriptCamera* instance, ScriptRenderTarget* target)
	{
		if (target == nullptr)
			instance->mCamera->getViewport()->setTarget(nullptr);
		else
			instance->mCamera->getViewport()->setTarget(target->getNativeValue());
	}

	bool ScriptCamera::internal_GetMain(ScriptCamera* instance)
	{
		return instance->mCamera->isMain();
	}

	void ScriptCamera::internal_SetMain(ScriptCamera* instance, bool main)
	{
		instance->mCamera->setMain(main);
		gSceneManager()._notifyMainCameraStateChanged(instance->mCamera);
	}

	void ScriptCamera::internal_UpdateView(ScriptCamera* instance, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		instance->updateView(parentSO);
	}

	void ScriptCamera::internal_OnDestroy(ScriptCamera* instance)
	{
		instance->destroy();
	}

	void ScriptCamera::destroy()
	{
		if (mCamera->isDestroyed())
			return;

		gSceneManager()._unregisterCamera(mCamera);
		mCamera->destroy();
	}

	void ScriptCamera::_onManagedInstanceDeleted()
	{
		destroy();

		ScriptObject::_onManagedInstanceDeleted();
	}
}
