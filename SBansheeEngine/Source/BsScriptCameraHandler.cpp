#include "BsScriptCameraHandler.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsApplication.h"
#include "BsCameraHandler.h"
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsScriptRenderTarget.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	ScriptCameraHandler::ScriptCameraHandler(MonoObject* managedInstance, const HSceneObject& parentSO)
		:ScriptObject(managedInstance), mCameraHandler(nullptr), mLastUpdateHash(0)
	{ 
		ViewportPtr primaryViewport = gApplication().getPrimaryViewport();
		RenderTargetPtr target;

		if (primaryViewport != nullptr) // TODO - Normally this should never be null, but right now there is no primary viewport
			target = primaryViewport->getTarget();

		mCameraHandler = CameraHandler::create(target);
		gSceneManager()._registerCamera(mCameraHandler, parentSO);
	}

	ScriptCameraHandler::~ScriptCameraHandler()
	{

	}

	void ScriptCameraHandler::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptCameraHandler::internal_Create);

		metaData.scriptClass->addInternalCall("Internal_GetAspect", &ScriptCameraHandler::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetAspect", &ScriptCameraHandler::internal_SetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetNearClip", &ScriptCameraHandler::internal_GetNearClip);
		metaData.scriptClass->addInternalCall("Internal_SetNearClip", &ScriptCameraHandler::internal_SetNearClip);

		metaData.scriptClass->addInternalCall("Internal_GetFarClip", &ScriptCameraHandler::internal_GetFarClip);
		metaData.scriptClass->addInternalCall("Internal_SetFarClip", &ScriptCameraHandler::internal_SetFarClip);

		metaData.scriptClass->addInternalCall("Internal_GetFieldOfView", &ScriptCameraHandler::internal_GetFieldOfView);
		metaData.scriptClass->addInternalCall("Internal_SetFieldOfView", &ScriptCameraHandler::internal_SetFieldOfView);

		metaData.scriptClass->addInternalCall("Internal_GetViewportRect", &ScriptCameraHandler::internal_GetViewportRect);
		metaData.scriptClass->addInternalCall("Internal_SetViewportRect", &ScriptCameraHandler::internal_SetViewportRect);

		metaData.scriptClass->addInternalCall("Internal_GetProjectionType", &ScriptCameraHandler::internal_GetProjectionType);
		metaData.scriptClass->addInternalCall("Internal_SetProjectionType", &ScriptCameraHandler::internal_SetProjectionType);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicHeight", &ScriptCameraHandler::internal_GetOrthographicHeight);
		metaData.scriptClass->addInternalCall("Internal_SetOrthographicHeight", &ScriptCameraHandler::internal_SetOrthographicHeight);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicWidth", &ScriptCameraHandler::internal_GetOrthographicWidth);

		metaData.scriptClass->addInternalCall("Internal_GetClearColor", &ScriptCameraHandler::internal_GetClearColor);
		metaData.scriptClass->addInternalCall("Internal_SetClearColor", &ScriptCameraHandler::internal_SetClearColor);

		metaData.scriptClass->addInternalCall("Internal_GetDepthClearValue", &ScriptCameraHandler::internal_GetDepthClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetDepthClearValue", &ScriptCameraHandler::internal_SetDepthClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetStencilClearValue", &ScriptCameraHandler::internal_GetStencilClearValue);
		metaData.scriptClass->addInternalCall("Internal_SetStencilClearValue", &ScriptCameraHandler::internal_SetStencilClearValue);

		metaData.scriptClass->addInternalCall("Internal_GetClearFlags", &ScriptCameraHandler::internal_GetClearFlags);
		metaData.scriptClass->addInternalCall("Internal_SetClearFlags", &ScriptCameraHandler::internal_SetClearFlags);

		metaData.scriptClass->addInternalCall("Internal_GetPriority", &ScriptCameraHandler::internal_GetPriority);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", &ScriptCameraHandler::internal_SetPriority);

		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptCameraHandler::internal_GetLayers);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptCameraHandler::internal_SetLayers);

		metaData.scriptClass->addInternalCall("Internal_GetProjMatrix", &ScriptCameraHandler::internal_GetProjMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetProjMatrixInv", &ScriptCameraHandler::internal_GetProjMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetViewMatrix", &ScriptCameraHandler::internal_GetViewMatrix);
		metaData.scriptClass->addInternalCall("Internal_GetViewMatrixInv", &ScriptCameraHandler::internal_GetViewMatrixInv);

		metaData.scriptClass->addInternalCall("Internal_GetWidthPixels", &ScriptCameraHandler::internal_GetWidthPixels);
		metaData.scriptClass->addInternalCall("Internal_GetHeightPixels", &ScriptCameraHandler::internal_GetHeightPixels);

		metaData.scriptClass->addInternalCall("Internal_WorldToScreen", &ScriptCameraHandler::internal_WorldToScreen);
		metaData.scriptClass->addInternalCall("Internal_WorldToClip", &ScriptCameraHandler::internal_WorldToClip);
		metaData.scriptClass->addInternalCall("Internal_WorldToView", &ScriptCameraHandler::internal_WorldToView);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorld", &ScriptCameraHandler::internal_ScreenToWorld);
		metaData.scriptClass->addInternalCall("Internal_ScreenToView", &ScriptCameraHandler::internal_ScreenToView);
		metaData.scriptClass->addInternalCall("Internal_ScreenToClip", &ScriptCameraHandler::internal_ScreenToClip);

		metaData.scriptClass->addInternalCall("Internal_ViewToWorld", &ScriptCameraHandler::internal_ViewToWorld);
		metaData.scriptClass->addInternalCall("Internal_ViewToScreen", &ScriptCameraHandler::internal_ViewToScreen);
		metaData.scriptClass->addInternalCall("Internal_ViewToClip", &ScriptCameraHandler::internal_ViewToClip);

		metaData.scriptClass->addInternalCall("Internal_ClipToWorld", &ScriptCameraHandler::internal_ClipToWorld);
		metaData.scriptClass->addInternalCall("Internal_ClipToView", &ScriptCameraHandler::internal_ClipToView);
		metaData.scriptClass->addInternalCall("Internal_ClipToScreen", &ScriptCameraHandler::internal_ClipToScreen);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorldRay", &ScriptCameraHandler::internal_ScreenToWorldRay);
		metaData.scriptClass->addInternalCall("Internal_ProjectPoint", &ScriptCameraHandler::internal_ProjectPoint);
		metaData.scriptClass->addInternalCall("Internal_UnprojectPoint", &ScriptCameraHandler::internal_UnprojectPoint);

		metaData.scriptClass->addInternalCall("Internal_SetRenderTarget", &ScriptCameraHandler::internal_SetRenderTarget);

		metaData.scriptClass->addInternalCall("Internal_UpdateView", &ScriptCameraHandler::internal_UpdateView);
		metaData.scriptClass->addInternalCall("Internal_OnDestroy", &ScriptCameraHandler::internal_OnDestroy);
	}

	void ScriptCameraHandler::updateView(const HSceneObject& parent)
	{
		if (parent.isDestroyed())
			return;

		UINT32 curHash = parent->getTransformHash();
		if (curHash != mLastUpdateHash)
		{
			mCameraHandler->setPosition(parent->getWorldPosition());
			mCameraHandler->setRotation(parent->getWorldRotation());

			mLastUpdateHash = curHash;
		}
	}

	void ScriptCameraHandler::internal_Create(MonoObject* managedInstance, ScriptSceneObject* parentSO)
	{
		HSceneObject so;
		if (parentSO != nullptr)
			so = parentSO->getNativeHandle();

		ScriptCameraHandler* nativeInstance = new (bs_alloc<ScriptCameraHandler>()) ScriptCameraHandler(managedInstance, so);
	}

	float ScriptCameraHandler::internal_GetAspect(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getAspectRatio();
	}

	void ScriptCameraHandler::internal_SetAspect(ScriptCameraHandler* instance, float value)
	{
		instance->mCameraHandler->setAspectRatio(value);
	}

	float ScriptCameraHandler::internal_GetNearClip(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getNearClipDistance();
	}

	void ScriptCameraHandler::internal_SetNearClip(ScriptCameraHandler* instance, float value)
	{
		instance->mCameraHandler->setNearClipDistance(value);
	}

	float ScriptCameraHandler::internal_GetFarClip(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getFarClipDistance();
	}

	void ScriptCameraHandler::internal_SetFarClip(ScriptCameraHandler* instance, float value)
	{
		instance->mCameraHandler->setFarClipDistance(value);
	}

	Degree ScriptCameraHandler::internal_GetFieldOfView(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getHorzFOV();
	}

	void ScriptCameraHandler::internal_SetFieldOfView(ScriptCameraHandler* instance, Degree value)
	{
		instance->mCameraHandler->setHorzFOV(value);
	}

	Rect2 ScriptCameraHandler::internal_GetViewportRect(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewport()->getNormArea();
	}

	void ScriptCameraHandler::internal_SetViewportRect(ScriptCameraHandler* instance, Rect2 value)
	{
		instance->mCameraHandler->getViewport()->setArea(value.x, value.y, value.width, value.height);
	}

	UINT32 ScriptCameraHandler::internal_GetProjectionType(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getProjectionType();
	}

	void ScriptCameraHandler::internal_SetProjectionType(ScriptCameraHandler* instance, UINT32 value)
	{
		instance->mCameraHandler->setProjectionType((ProjectionType)value);
	}

	float ScriptCameraHandler::internal_GetOrthographicHeight(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getOrthoWindowHeight();
	}

	void ScriptCameraHandler::internal_SetOrthographicHeight(ScriptCameraHandler* instance, float value)
	{
		instance->mCameraHandler->setOrthoWindowHeight(value);
	}

	float ScriptCameraHandler::internal_GetOrthographicWidth(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getOrthoWindowWidth();
	}

	Color ScriptCameraHandler::internal_GetClearColor(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearColor();
	}

	void ScriptCameraHandler::internal_SetClearColor(ScriptCameraHandler* instance, Color value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(value, vp->getClearDepthValue(), vp->getClearStencilValue());
	}

	float ScriptCameraHandler::internal_GetDepthClearValue(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearDepthValue();
	}

	void ScriptCameraHandler::internal_SetDepthClearValue(ScriptCameraHandler* instance, float value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(vp->getClearColor(), value, vp->getClearStencilValue());
	}

	UINT16 ScriptCameraHandler::internal_GetStencilClearValue(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearStencilValue();
	}

	void ScriptCameraHandler::internal_SetStencilClearValue(ScriptCameraHandler* instance, UINT16 value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(vp->getClearColor(), vp->getClearDepthValue(), value);
	}

	UINT32 ScriptCameraHandler::internal_GetClearFlags(ScriptCameraHandler* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		UINT32 clearFlags = 0;

		clearFlags |= vp->getRequiresColorClear() ? 0x01 : 0;
		clearFlags |= vp->getRequiresDepthClear() ? 0x02 : 0;
		clearFlags |= vp->getRequiresStencilClear() ? 0x04 : 0;

		return clearFlags;
	}

	void ScriptCameraHandler::internal_SetClearFlags(ScriptCameraHandler* instance, UINT32 value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		vp->setRequiresClear((value & 0x01) != 0,
			(value & 0x02) != 0, (value & 0x04) != 0);
	}

	int ScriptCameraHandler::internal_GetPriority(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getPriority();
	}

	void ScriptCameraHandler::internal_SetPriority(ScriptCameraHandler* instance, int value)
	{
		instance->mCameraHandler->setPriority(value);
	}

	UINT64 ScriptCameraHandler::internal_GetLayers(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getLayers();
	}

	void ScriptCameraHandler::internal_SetLayers(ScriptCameraHandler* instance, UINT64 value)
	{
		instance->mCameraHandler->setLayers(value);
	}

	Matrix4 ScriptCameraHandler::internal_GetProjMatrix(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getProjectionMatrixRS();
	}

	Matrix4 ScriptCameraHandler::internal_GetProjMatrixInv(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getProjectionMatrixRSInv();
	}

	Matrix4 ScriptCameraHandler::internal_GetViewMatrix(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewMatrix();
	}

	Matrix4 ScriptCameraHandler::internal_GetViewMatrixInv(ScriptCameraHandler* instance)
	{
		return instance->mCameraHandler->getViewMatrixInv();
	}

	int ScriptCameraHandler::internal_GetWidthPixels(ScriptCameraHandler* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		return vp->getWidth();
	}

	int ScriptCameraHandler::internal_GetHeightPixels(ScriptCameraHandler* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		return vp->getHeight();
	}

	Vector2I ScriptCameraHandler::internal_WorldToScreen(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToScreenPoint(value);
	}

	Vector2 ScriptCameraHandler::internal_WorldToClip(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToClipPoint(value);
	}

	Vector3 ScriptCameraHandler::internal_WorldToView(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToViewPoint(value);
	}

	Vector3 ScriptCameraHandler::internal_ScreenToWorld(ScriptCameraHandler* instance, Vector2I value, float depth)
	{
		return instance->mCameraHandler->screenToWorldPoint(value, depth);
	}

	Vector3 ScriptCameraHandler::internal_ScreenToView(ScriptCameraHandler* instance, Vector2I value, float depth)
	{
		return instance->mCameraHandler->screenToViewPoint(value, depth);
	}

	Vector2 ScriptCameraHandler::internal_ScreenToClip(ScriptCameraHandler* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenToClipPoint(value);
	}

	Vector3 ScriptCameraHandler::internal_ViewToWorld(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToWorldPoint(value);
	}

	Vector2I ScriptCameraHandler::internal_ViewToScreen(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToScreenPoint(value);
	}

	Vector2 ScriptCameraHandler::internal_ViewToClip(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToClipPoint(value);
	}

	Vector3 ScriptCameraHandler::internal_ClipToWorld(ScriptCameraHandler* instance, Vector2 value, float depth)
	{
		return instance->mCameraHandler->clipToWorldPoint(value, depth);
	}

	Vector3 ScriptCameraHandler::internal_ClipToView(ScriptCameraHandler* instance, Vector2 value, float depth)
	{
		return instance->mCameraHandler->clipToViewPoint(value, depth);
	}

	Vector2I ScriptCameraHandler::internal_ClipToScreen(ScriptCameraHandler* instance, Vector2 value)
	{
		return instance->mCameraHandler->clipToScreenPoint(value);
	}

	Ray ScriptCameraHandler::internal_ScreenToWorldRay(ScriptCameraHandler* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenPointToRay(value);
	}

	Vector3 ScriptCameraHandler::internal_ProjectPoint(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->projectPoint(value);
	}

	Vector3 ScriptCameraHandler::internal_UnprojectPoint(ScriptCameraHandler* instance, Vector3 value)
	{
		return instance->mCameraHandler->unprojectPoint(value);
	}

	void ScriptCameraHandler::internal_SetRenderTarget(ScriptCameraHandler* instance, ScriptRenderTarget* target)
	{
		if (target == nullptr)
		{
			ViewportPtr primaryViewport = gApplication().getPrimaryViewport();

			instance->mCameraHandler->getViewport()->setTarget(primaryViewport->getTarget());
		}
		else
		{
			instance->mCameraHandler->getViewport()->setTarget(target->getNativeValue());
		}
	}

	void ScriptCameraHandler::internal_UpdateView(ScriptCameraHandler* instance, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		instance->updateView(parentSO);
	}

	void ScriptCameraHandler::internal_OnDestroy(ScriptCameraHandler* instance)
	{
		gSceneManager()._unregisterCamera(instance->getInternal());
		instance->getInternal()->destroy();
	}
}