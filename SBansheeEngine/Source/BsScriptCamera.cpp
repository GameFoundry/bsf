#include "BsScriptCamera.h"
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

namespace BansheeEngine
{
	ScriptCamera::ScriptCamera(MonoObject* managedInstance)
		:ScriptObject(managedInstance), mCameraHandler(nullptr), mLastUpdateHash(0)
	{ 
		ViewportPtr primaryViewport = gApplication().getPrimaryViewport();

		mCameraHandler = CameraHandler::create(primaryViewport->getTarget());
	}

	ScriptCamera::~ScriptCamera()
	{

	}

	void ScriptCamera::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptCamera::internal_Create);

		metaData.scriptClass->addInternalCall("Internal_GetAspect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetAspect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetNearClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetNearClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetFarClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetFarClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetFieldOfView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetFieldOfView", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetViewportRect", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetViewportRect", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetProjectionType", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetProjectionType", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicHeight", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetOrthographicHeight", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetOrthographicWidth", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetClearColor", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetClearColor", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetDepthClearValue", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetDepthClearValue", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetStencilClearValue", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetStencilClearValue", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetClearFlags", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetClearFlags", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetPriority", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetLayers", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_SetLayers", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetProjMatrix", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetProjMatrixInv", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetViewMatrix", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetViewMatrixInv", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_GetWidthPixels", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_GetHeightPixels", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_WorldToScreen", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_WorldToClip", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_WorldToView", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ScreenToView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ScreenToClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ViewToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ViewToScreen", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ViewToClip", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ClipToWorld", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ClipToView", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ClipToScreen", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_ScreenToWorldRay", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_ProjectPoint", &ScriptCamera::internal_GetAspect);
		metaData.scriptClass->addInternalCall("Internal_UnprojectPoint", &ScriptCamera::internal_GetAspect);

		metaData.scriptClass->addInternalCall("Internal_SetRenderTarget", &ScriptCamera::internal_SetRenderTarget);

		metaData.scriptClass->addInternalCall("Internal_UpdateView", &ScriptCamera::internal_UpdateView);
	}

	void ScriptCamera::updateView(const HSceneObject& parent)
	{
		UINT32 curHash = parent->getTransformHash();
		if (curHash != mLastUpdateHash)
		{
			mCameraHandler->setPosition(parent->getWorldPosition());
			mCameraHandler->setRotation(parent->getWorldRotation());

			mLastUpdateHash = curHash;
		}
	}

	void ScriptCamera::internal_Create(MonoObject* managedInstance)
	{
		ScriptCamera* nativeInstance = new (bs_alloc<ScriptCamera>()) ScriptCamera(managedInstance);
	}

	float ScriptCamera::internal_GetAspect(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getAspectRatio();
	}

	void ScriptCamera::internal_SetAspect(ScriptCamera* instance, float value)
	{
		instance->mCameraHandler->setAspectRatio(value);
	}

	float ScriptCamera::internal_GetNearClip(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getNearClipDistance();
	}

	void ScriptCamera::internal_SetNearClip(ScriptCamera* instance, float value)
	{
		instance->mCameraHandler->setNearClipDistance(value);
	}

	float ScriptCamera::internal_GetFarClip(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getFarClipDistance();
	}

	void ScriptCamera::internal_SetFarClip(ScriptCamera* instance, float value)
	{
		instance->mCameraHandler->setFarClipDistance(value);
	}

	Degree ScriptCamera::internal_GetFieldOfView(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getHorzFOV();
	}

	void ScriptCamera::internal_SetFieldOfView(ScriptCamera* instance, Degree value)
	{
		instance->mCameraHandler->setHorzFOV(value);
	}

	Rect2 ScriptCamera::internal_GetViewportRect(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewport()->getNormArea();
	}

	void ScriptCamera::internal_SetViewportRect(ScriptCamera* instance, Rect2 value)
	{
		instance->mCameraHandler->getViewport()->setArea(value.x, value.y, value.width, value.height);
	}

	UINT32 ScriptCamera::internal_GetProjectionType(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getProjectionType();
	}

	void ScriptCamera::internal_SetProjectionType(ScriptCamera* instance, UINT32 value)
	{
		instance->mCameraHandler->setProjectionType((ProjectionType)value);
	}

	float ScriptCamera::internal_GetOrthographicHeight(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getOrthoWindowHeight();
	}

	void ScriptCamera::internal_SetOrthographicHeight(ScriptCamera* instance, float value)
	{
		instance->mCameraHandler->setOrthoWindowHeight(value);
	}

	float ScriptCamera::internal_GetOrthographicWidth(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getOrthoWindowWidth();
	}

	Color ScriptCamera::internal_GetClearColor(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearColor();
	}

	void ScriptCamera::internal_SetClearColor(ScriptCamera* instance, Color value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(value, vp->getClearDepthValue(), vp->getClearStencilValue());
	}

	float ScriptCamera::internal_GetDepthClearValue(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearDepthValue();
	}

	void ScriptCamera::internal_SetDepthClearValue(ScriptCamera* instance, float value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(vp->getClearColor(), value, vp->getClearStencilValue());
	}

	UINT16 ScriptCamera::internal_GetStencilClearValue(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewport()->getClearStencilValue();
	}

	void ScriptCamera::internal_SetStencilClearValue(ScriptCamera* instance, UINT16 value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		vp->setClearValues(vp->getClearColor(), vp->getClearDepthValue(), value);
	}

	UINT32 ScriptCamera::internal_GetClearFlags(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();
		UINT32 clearFlags = 0;

		clearFlags |= vp->getRequiresColorClear() ? 0x01 : 0;
		clearFlags |= vp->getRequiresDepthClear() ? 0x02 : 0;
		clearFlags |= vp->getRequiresStencilClear() ? 0x04 : 0;

		return clearFlags;
	}

	void ScriptCamera::internal_SetClearFlags(ScriptCamera* instance, UINT32 value)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		vp->setRequiresClear((value & 0x01) != 0,
			(value & 0x02) != 0, (value & 0x04) != 0);
	}

	int ScriptCamera::internal_GetPriority(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getPriority();
	}

	void ScriptCamera::internal_SetPriority(ScriptCamera* instance, int value)
	{
		instance->mCameraHandler->setPriority(value);
	}

	UINT64 ScriptCamera::internal_GetLayers(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getLayers();
	}

	void ScriptCamera::internal_SetLayers(ScriptCamera* instance, UINT64 value)
	{
		instance->mCameraHandler->setLayers(value);
	}

	Matrix4 ScriptCamera::internal_GetProjMatrix(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getProjectionMatrixRS();
	}

	Matrix4 ScriptCamera::internal_GetProjMatrixInv(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getProjectionMatrixRSInv();
	}

	Matrix4 ScriptCamera::internal_GetViewMatrix(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewMatrix();
	}

	Matrix4 ScriptCamera::internal_GetViewMatrixInv(ScriptCamera* instance)
	{
		return instance->mCameraHandler->getViewMatrixInv();
	}

	int ScriptCamera::internal_GetWidthPixels(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		return vp->getWidth();
	}

	int ScriptCamera::internal_GetHeightPixels(ScriptCamera* instance)
	{
		ViewportPtr vp = instance->mCameraHandler->getViewport();

		return vp->getHeight();
	}

	Vector2I ScriptCamera::internal_WorldToScreen(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToScreenPoint(value);
	}

	Vector2 ScriptCamera::internal_WorldToClip(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_WorldToView(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->worldToViewPoint(value);
	}

	Vector3 ScriptCamera::internal_ScreenToWorld(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenToWorldPoint(value);
	}

	Vector3 ScriptCamera::internal_ScreenToView(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenToViewPoint(value);
	}

	Vector2 ScriptCamera::internal_ScreenToClip(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_ViewToWorld(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToWorldPoint(value);
	}

	Vector2I ScriptCamera::internal_ViewToScreen(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToScreenPoint(value);
	}

	Vector2 ScriptCamera::internal_ViewToClip(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->viewToClipPoint(value);
	}

	Vector3 ScriptCamera::internal_ClipToWorld(ScriptCamera* instance, Vector2 value)
	{
		return instance->mCameraHandler->clipToWorldPoint(value);
	}

	Vector3 ScriptCamera::internal_ClipToView(ScriptCamera* instance, Vector2 value)
	{
		return instance->mCameraHandler->clipToViewPoint(value);
	}

	Vector2I ScriptCamera::internal_ClipToScreen(ScriptCamera* instance, Vector2 value)
	{
		return instance->mCameraHandler->clipToScreenPoint(value);
	}

	Ray ScriptCamera::internal_ScreenToWorldRay(ScriptCamera* instance, Vector2I value)
	{
		return instance->mCameraHandler->screenPointToRay(value);
	}

	Vector3 ScriptCamera::internal_ProjectPoint(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->projectPoint(value);
	}

	Vector3 ScriptCamera::internal_UnprojectPoint(ScriptCamera* instance, Vector3 value)
	{
		return instance->mCameraHandler->unprojectPoint(value);
	}

	void ScriptCamera::internal_SetRenderTarget(ScriptCamera* instance, ScriptRenderTarget* target)
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

	void ScriptCamera::internal_UpdateView(ScriptCamera* instance, ScriptSceneObject* parent)
	{
		HSceneObject parentSO = parent->getNativeSceneObject();

		instance->updateView(parentSO);
	}
}