//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptInput.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsInput.h"
#include "BsScriptVector2I.h"
#include "BsPlayInEditorManager.h"
#include "BsSceneManager.h"
#include "BsCamera.h"
#include "BsViewport.h"
#include "BsRenderTarget.h"
#include "BsRenderWindow.h"
#include "BsRenderTexture.h"
#include "BsGUIManager.h"

namespace BansheeEngine
{
	ScriptInput::OnButtonEventThunkDef ScriptInput::OnButtonPressedThunk;
	ScriptInput::OnButtonEventThunkDef ScriptInput::OnButtonReleasedThunk;
	ScriptInput::OnCharInputEventThunkDef ScriptInput::OnCharInputThunk;
	ScriptInput::OnPointerEventThunkDef ScriptInput::OnPointerPressedThunk;
	ScriptInput::OnPointerEventThunkDef ScriptInput::OnPointerReleasedThunk;
	ScriptInput::OnPointerEventThunkDef ScriptInput::OnPointerMovedThunk;
	ScriptInput::OnPointerEventThunkDef ScriptInput::OnPointerDoubleClickThunk;

	HEvent ScriptInput::OnButtonPressedConn;
	HEvent ScriptInput::OnButtonReleasedConn;
	HEvent ScriptInput::OnCharInputConn;
	HEvent ScriptInput::OnPointerPressedConn;
	HEvent ScriptInput::OnPointerReleasedConn;
	HEvent ScriptInput::OnPointerMovedConn;
	HEvent ScriptInput::OnPointerDoubleClickConn;

	ScriptInput::ScriptInput(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptInput::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_IsButtonHeld", &ScriptInput::internal_isButtonHeld);
		metaData.scriptClass->addInternalCall("Internal_IsButtonDown", &ScriptInput::internal_isButtonDown);
		metaData.scriptClass->addInternalCall("Internal_IsButtonUp", &ScriptInput::internal_isButtonUp);
		metaData.scriptClass->addInternalCall("Internal_IsPointerButtonHeld", &ScriptInput::internal_isPointerButtonHeld);
		metaData.scriptClass->addInternalCall("Internal_IsPointerButtonDown", &ScriptInput::internal_isPointerButtonDown);
		metaData.scriptClass->addInternalCall("Internal_IsPointerButtonUp", &ScriptInput::internal_isPointerButtonUp);
		metaData.scriptClass->addInternalCall("Internal_IsPointerDoubleClicked", &ScriptInput::internal_isPointerDoubleClicked);
		metaData.scriptClass->addInternalCall("Internal_GetAxisValue", &ScriptInput::internal_getAxisValue);
		metaData.scriptClass->addInternalCall("Internal_GetPointerPosition", &ScriptInput::internal_getPointerPosition);
		metaData.scriptClass->addInternalCall("Internal_GetPointerScreenPosition", &ScriptInput::internal_getPointerScreenPosition);
		metaData.scriptClass->addInternalCall("Internal_GetPointerDelta", &ScriptInput::internal_getPointerDelta);

		OnButtonPressedThunk = (OnButtonEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerButtonDown", "ButtonCode,int")->getThunk();
		OnButtonReleasedThunk = (OnButtonEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerButtonUp", "ButtonCode,int")->getThunk();
		OnCharInputThunk = (OnCharInputEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerCharInput", "int")->getThunk();
		OnPointerPressedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerPressed", "Vector2I,Vector2I,PointerButton,bool,bool,bool,single")->getThunk();
		OnPointerReleasedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerReleased", "Vector2I,Vector2I,PointerButton,bool,bool,bool,single")->getThunk();
		OnPointerMovedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerMove", "Vector2I,Vector2I,PointerButton,bool,bool,bool,single")->getThunk();
		OnPointerDoubleClickThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerDoubleClick", "Vector2I,Vector2I,PointerButton,bool,bool,bool,single")->getThunk();
	}

	void ScriptInput::startUp()
	{
		Input& input = Input::instance();

		OnButtonPressedConn = input.onButtonDown.connect(&ScriptInput::onButtonDown);
		OnButtonReleasedConn = input.onButtonUp.connect(&ScriptInput::onButtonUp);
		OnCharInputConn = input.onCharInput.connect(&ScriptInput::onCharInput);
		OnPointerPressedConn = input.onPointerPressed.connect(&ScriptInput::onPointerPressed);
		OnPointerReleasedConn = input.onPointerReleased.connect(&ScriptInput::onPointerReleased);
		OnPointerMovedConn = input.onPointerMoved.connect(&ScriptInput::onPointerMoved);
		OnPointerDoubleClickConn = input.onPointerDoubleClick.connect(&ScriptInput::onPointerDoubleClick);
	}

	void ScriptInput::shutDown()
	{
		OnButtonPressedConn.disconnect();
		OnButtonReleasedConn.disconnect();
		OnCharInputConn.disconnect();
		OnPointerPressedConn.disconnect();
		OnPointerReleasedConn.disconnect();
		OnPointerMovedConn.disconnect();
		OnPointerDoubleClickConn.disconnect();
	}

	void ScriptInput::onButtonDown(const ButtonEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoUtil::invokeThunk(OnButtonPressedThunk, ev.buttonCode, ev.deviceIdx);
	}

	void ScriptInput::onButtonUp(const ButtonEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoUtil::invokeThunk(OnButtonReleasedThunk, ev.buttonCode, ev.deviceIdx);
	}

	void ScriptInput::onCharInput(const TextInputEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoUtil::invokeThunk(OnCharInputThunk, ev.textChar);
	}

	void ScriptInput::onPointerMoved(const PointerEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);
		MonoObject* delta = ScriptVector2I::box(ev.delta);

		MonoUtil::invokeThunk(OnPointerMovedThunk, screenPos, delta, 
			ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount);
	}

	void ScriptInput::onPointerPressed(const PointerEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);
		MonoObject* delta = ScriptVector2I::box(ev.delta);

		MonoUtil::invokeThunk(OnPointerPressedThunk, screenPos, delta, 
			ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount);
	}

	void ScriptInput::onPointerReleased(const PointerEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);
		MonoObject* delta = ScriptVector2I::box(ev.delta);

		MonoUtil::invokeThunk(OnPointerReleasedThunk, screenPos, delta, 
			ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount);
	}

	void ScriptInput::onPointerDoubleClick(const PointerEvent& ev)
	{
		if (PlayInEditorManager::instance().getState() != PlayInEditorState::Playing)
			return;

		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);
		MonoObject* delta = ScriptVector2I::box(ev.delta);

		MonoUtil::invokeThunk(OnPointerDoubleClickThunk, screenPos, delta, 
			ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount);
	}

	bool ScriptInput::internal_isButtonHeld(ButtonCode code, UINT32 deviceIdx)
	{
		return Input::instance().isButtonHeld(code, deviceIdx);
	}

	bool ScriptInput::internal_isButtonDown(ButtonCode code, UINT32 deviceIdx)
	{
		return Input::instance().isButtonDown(code, deviceIdx);
	}

	bool ScriptInput::internal_isButtonUp(ButtonCode code, UINT32 deviceIdx)
	{
		return Input::instance().isButtonUp(code, deviceIdx);
	}

	bool ScriptInput::internal_isPointerButtonHeld(PointerEventButton code)
	{
		return Input::instance().isPointerButtonHeld(code);
	}

	bool ScriptInput::internal_isPointerButtonDown(PointerEventButton code)
	{
		return Input::instance().isPointerButtonDown(code);
	}

	bool ScriptInput::internal_isPointerButtonUp(PointerEventButton code)
	{
		return Input::instance().isPointerButtonUp(code);
	}

	bool ScriptInput::internal_isPointerDoubleClicked()
	{
		return Input::instance().isPointerDoubleClicked();
	}

	float ScriptInput::internal_getAxisValue(UINT32 axisType, UINT32 deviceIdx)
	{
		return Input::instance().getAxisValue(axisType, deviceIdx);
	}

	void ScriptInput::internal_getPointerPosition(Vector2I* position)
	{
		*position = Input::instance().getPointerPosition();

		SceneCameraData mainCamera = gSceneManager().getMainCamera();
		if (mainCamera.camera == nullptr)
			return;

		// The main camera could be rendering to a standalone window, or be a part of the editor GUI. Find out which
		// and transform the pointer position appropriately.
		RenderTargetPtr target = mainCamera.camera->getViewport()->getTarget();
		if (target == nullptr)
			return;

		if(target->getProperties().isWindow())
		{
			RenderWindowPtr window = std::static_pointer_cast<RenderWindow>(target);
			*position = window->screenToWindowPos(*position);
		}
		else
		{
			RenderTexturePtr texture = std::static_pointer_cast<RenderTexture>(target);

			RenderWindowPtr window = GUIManager::instance().getBridgeWindow(texture);
			if (window == nullptr)
				return;

			*position = window->screenToWindowPos(*position);
			*position = GUIManager::instance().windowToBridgedCoords(target, *position);
		}		
	}

	void ScriptInput::internal_getPointerScreenPosition(Vector2I* position)
	{
		*position = Input::instance().getPointerPosition();
	}

	void ScriptInput::internal_getPointerDelta(Vector2I* position)
	{
		*position = Input::instance().getPointerDelta();
	}
}