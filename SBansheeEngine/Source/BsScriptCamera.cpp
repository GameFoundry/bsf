#include "BsScriptCamera.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsApplication.h"
#include "BsCamera.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsScriptRenderTarget.h"
#include "BsSceneManager.h"

namespace BansheeEngine
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
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptCamera::internal_Create);

		metaData.scriptClass->addInternalCall("Internal_GetAspect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetAspect", &ScriptCamera::internal_SetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetNearClip", &ScriptCamera::internal_GetNearClip);
		metaData.scriptClass->addInternalCall("Internal_SetNearClip", &ScriptCamera::internal_SetNearClip);

		metaData.scriptClass->addInternalCall("Internal_GetFarClip", &ScriptCamera::internal_GetFarClip);
		metaData.scriptClass->addInternalCall("Internal_SetFarClip", &ScriptCamera::internal_SetFarClip);

		metaData.scriptClass->addInternalCall("Internal_GetFieldOfView", &ScriptCamera::internal_GetFieldOfView);
		metaData.scriptClass->addInternalCall("Internal_SetFieldOfView", &ScriptCamera::internal_SetFieldOfView);

		metaData.scriptClass->addInternalCall("Internal_GetViewportRect", &ScriptCamera::internal_GetViewportRect);
		metaData.scriptClass->addInternalCall("Internal_SetViewportRect", &ScriptCamera::internal_SetViewportRect);

		metaData.scriptClass->addInternalCall("Internal_GetProjectionType", &ScriptCamera::internal_GetProjectionType);
		metaData.scriptClass->addInternalCall("Internal_SetProjectionType", &ScriptCamera::internal_SetProjectionType);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicHeight", &ScriptCamera::internal_GetOrthographicHeight);
		metaData.scriptClass->addInternalCall("Internal_SetOrthographicHeight", &ScriptCamera::internal_SetOrthographicHeight);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicWidth", &ScriptCamera::internal_GetOrthographicWidth);

		metaData.scriptClass->addInternalCall("Internal_GetClearColor", &ScriptCamera::internal_GetClearColor);
		metaData.scriptClass->addInternalCall("Internal_SetClearColor", &ScriptCamera::internal_SetClearColor);

		metaData.scriptClass->addInternalCall("Internal_GetDepthClearValue", &ScriptCamera::internal_GetDepthClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetDepthClearValue", &ScriptCamera::internal_SetDepthClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetStencilClearValue", &ScriptCamera::internal_GetStencilClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetStencilClearValue", &ScriptCamera::internal_SetStencilClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetClearFlags", &ScriptCamera::internal_GetClearFlags);
		metaData.scriptClass->addInternalCall("Internal_SetClearFlags", &ScriptCamera::internal_SetClearFlags);

		metaData.scriptClass->addInternalCall("Internal_GetPriority", &ScriptCamera::internal_GetPriority);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", &ScriptCamera::internal_SetPriority);

		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptCamera::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptCamera::internal_SetLayers);

		metaData.scriptClass->addInternalCall("Internal_GetProjMatrix", &ScriptCamera::internal_GetProjMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetProjMatrixInv", &ScriptCamera::internal_GetProjMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetViewMatrix", &ScriptCamera::internal_GetViewMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetViewMatrixInv", &ScriptCamera::internal_GetViewMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetWidthPixels", &ScriptCamera::internal_GetWidthPixels);
		metaData.scriptClass->addInternalCall("Internal_GetHeightPixels", &ScriptCamera::internal_GetHeightPixels);

		metaData.scriptClass->addInternalCall("Internal_WorldToScreen", &ScriptCamera::internal_WorldToScreen);
		metaData.scriptClass->addInternalCall("Internal_WorldToClip", &ScriptCamera::internal_WorldToClip);
		metaData.scriptClass->addInternalCall("Internal_WorldToView", &ScriptCamera::internal_WorldToView);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorld", &ScriptCamera::internal_ScreenToWorld);
		metaData.scriptClass->addInternalCall("Internal_ScreenToView", &ScriptCamera::internal_ScreenToView);
		metaData.scriptClass->addInternalCall("Internal_ScreenToClip", &ScriptCamera::internal_ScreenToClip);

		metaData.scriptClass->addInternalCall("Internal_ViewToWorld", &ScriptCamera::internal_ViewToWorld);
		metaData.scriptClass->addInternalCall("Internal_ViewToScreen", &ScriptCamera::internal_ViewToScreen);
		metaData.scriptClass->addInternalCall("Internal_ViewToClip", &ScriptCamera::internal_ViewToClip);

		metaData.scriptClass->addInternalCall("Internal_ClipToWorld", &ScriptCamera::internal_ClipToWorld);
		metaData.scriptClass->addInternalCall("Internal_ClipToView", &ScriptCamera::internal_ClipToView);
		metaData.scriptClass->addInternalCall("Internal_ClipToScreen", &ScriptCamera::internal_ClipToScreen);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorldRay", &ScriptCamera::internal_ScreenToWorldRay);
		metaData.scriptClass->addInternalCall("Internal_ProjectPoint", &ScriptCamera::internal_ProjectPoint);
		metaData.scriptClass->addInternalCall("Internal_UnprojectPoint", &ScriptCamera::internal_UnprojectPoint);

		metaData.scriptClass->addInternalCall("Internal_SetRenderTarget", &ScriptCamera::internal_SetRenderTarget);

		metaData.scriptClass->addInternalCall("Internal_GetMain", &ScriptCamera::internal_GetMain);
		metaData.scriptClass->addInternalCall("Internal_SetMain", &ScriptCamera::internal_SetMain);

		metaData.scriptClass->addInternalCall("Internal_UpdateView", &ScriptCamera::internal_UpdateView);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptCamera::internal_OnDestroy);
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
	}

	void ScriptCamera::internal_Create(MonoObject* managedInstance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		ScriptCamera* nativeInstance = new (bs_alloc<ScriptCamera>()) ScriptCamera(managedInstance, so);
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

	void ScriptCamera::internal_SetFieldOfView(ScriptCamera* instance, Degree value)
	{
		instance->mCamera->setHorzFOV(value);
	}

	Rect2 ScriptCamera::internal_GetViewportRect(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getNormArea();
	}

	void ScriptCamera::internal_SetViewportRect(ScriptCamera* instance, Rect2 value)
	{
		instance->mCamera->getViewport()->setArea(value.x, value.y, value.width, value.height);
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

	Color ScriptCamera::internal_GetClearColor(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getClearColor();
	}

	void ScriptCamera::internal_SetClearColor(ScriptCamera* instance, Color value)
	{
		ViewportPtr vp = instance->mCamera->getViewport();
		vp->setClearValues(value, vp->getClearDepthValue(), vp->getClearStencilValue());
	}

	float ScriptCamera::internal_GetDepthClearValue(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getClearDepthValue();
	}

	void ScriptCamera::internal_SetDepthClearValue(ScriptCamera* instance, float value)
	{
		ViewportPtr vp = instance->mCamera->getViewport();
		vp->setClearValues(vp->getClearColor(), value, vp->getClearStencilValue());
	}

	UINT16 ScriptCamera::internal_GetStencilClearValue(ScriptCamera* instance)
	{
		return instance->mCamera->getViewport()->getClearStencilValue();
	}

	void ScriptCamera::internal_SetStencilClearValue(ScriptCamera* instance, UINT16 value)
	{
		ViewportPtr vp = instance->mCamera->getViewport();
		vp->setClearValues(vp->getClearColor(), vp->getClearDepthValue(), value);
	}

	UINT32 ScriptCamera::internal_GetClearFlags(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCamera->getViewport();
		UINT32 clearFlags = 0;

		clearFlags |= vp->getRequiresColorClear() ? 0x01 : 0;
		clearFlags |= vp->getRequiresDepthClear() ? 0x02 : 0;
		clearFlags |= vp->getRequiresStencilClear() ? 0x04 : 0;

		return clearFlags;
	}

	void ScriptCamera::internal_SetClearFlags(ScriptCamera* instance, UINT32 value)
	{
		ViewportPtr vp = instance->mCamera->getViewport();

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

	UINT64 ScriptCamera::internal_GetLayers(ScriptCamera* instance)
	{
		return instance->mCamera->getLayers();
	}

	void ScriptCamera::internal_SetLayers(ScriptCamera* instance, UINT64 value)
	{
		instance->mCamera->setLayers(value);
	}

	Matrix4 ScriptCamera::internal_GetProjMatrix(ScriptCamera* instance)
	{
		return instance->mCamera->getProjectionMatrixRS();
	}

	Matrix4 ScriptCamera::internal_GetProjMatrixInv(ScriptCamera* instance)
	{
		return instance->mCamera->getProjectionMatrixRSInv();
	}

	Matrix4 ScriptCamera::internal_GetViewMatrix(ScriptCamera* instance)
	{
		return instance->mCamera->getViewMatrix();
	}

	Matrix4 ScriptCamera::internal_GetViewMatrixInv(ScriptCamera* instance)
	{
		return instance->mCamera->getViewMatrixInv();
	}

	int ScriptCamera::internal_GetWidthPixels(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCamera->getViewport();

		return vp->getWidth();
	}

	int ScriptCamera::internal_GetHeightPixels(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCamera->getViewport();

		return vp->getHeight();
	}

	Vector2I ScriptCamera::internal_WorldToScreen(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->worldToScreenPoint(value);
	}

	Vector2 ScriptCamera::internal_WorldToClip(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->worldToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_WorldToView(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->worldToViewPoint(value);
	}

	Vector3 ScriptCamera::internal_ScreenToWorld(ScriptCamera* instance, Vector2I value, float depth)
	{
		return instance->mCamera->screenToWorldPoint(value, depth);
	}

	Vector3 ScriptCamera::internal_ScreenToView(ScriptCamera* instance, Vector2I value, float depth)
	{
		return instance->mCamera->screenToViewPoint(value, depth);
	}

	Vector2 ScriptCamera::internal_ScreenToClip(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCamera->screenToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_ViewToWorld(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->viewToWorldPoint(value);
	}

	Vector2I ScriptCamera::internal_ViewToScreen(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->viewToScreenPoint(value);
	}

	Vector2 ScriptCamera::internal_ViewToClip(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->viewToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_ClipToWorld(ScriptCamera* instance, Vector2 value, float depth)
	{
		return instance->mCamera->clipToWorldPoint(value, depth);
	}

	Vector3 ScriptCamera::internal_ClipToView(ScriptCamera* instance, Vector2 value, float depth)
	{
		return instance->mCamera->clipToViewPoint(value, depth);
	}

	Vector2I ScriptCamera::internal_ClipToScreen(ScriptCamera* instance, Vector2 value)
	{
		return instance->mCamera->clipToScreenPoint(value);
	}

	Ray ScriptCamera::internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCamera->screenPointToRay(value);
	}

	Vector3 ScriptCamera::internal_ProjectPoint(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->projectPoint(value);
	}

	Vector3 ScriptCamera::internal_UnprojectPoint(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCamera->unprojectPoint(value);
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