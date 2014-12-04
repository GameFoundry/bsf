#include "BsScriptInput.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsDebug.h"
#include "BsInput.h"
#include "BsScriptVector2I.h"

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
		metaData.scriptClass->addInternalCall("Internal_GetAxisValue", &ScriptInput::internal_getAxisValue);
		metaData.scriptClass->addInternalCall("Internal_GetPointerPosition", &ScriptInput::internal_getPointerPosition);

		OnButtonPressedThunk = (OnButtonEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerButtonDown", "ButtonCode,int")->getThunk();
		OnButtonReleasedThunk = (OnButtonEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerButtonUp", "ButtonCode,int")->getThunk();
		OnCharInputThunk = (OnCharInputEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerCharInput", "int")->getThunk();
		OnPointerPressedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerPressed", "Vector2I,PointerEventButton,bool,bool,bool,single")->getThunk();
		OnPointerReleasedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerReleased", "Vector2I,PointerEventButton,bool,bool,bool,single")->getThunk();
		OnPointerMovedThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerMove", "Vector2I,PointerEventButton,bool,bool,bool,single")->getThunk();
		OnPointerDoubleClickThunk = (OnPointerEventThunkDef)metaData.scriptClass->getMethodExact("Internal_TriggerPointerDoubleClick", "Vector2I,PointerEventButton,bool,bool,bool,single")->getThunk();
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
		MonoException* exception = nullptr;
		OnButtonPressedThunk(ev.buttonCode, ev.deviceIdx, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onButtonUp(const ButtonEvent& ev)
	{
		MonoException* exception = nullptr;
		OnButtonReleasedThunk(ev.buttonCode, ev.deviceIdx, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onCharInput(const TextInputEvent& ev)
	{
		MonoException* exception = nullptr;
		OnCharInputThunk(ev.textChar, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onPointerMoved(const PointerEvent& ev)
	{
		MonoException* exception = nullptr;
		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);

		OnPointerMovedThunk(screenPos, ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onPointerPressed(const PointerEvent& ev)
	{
		MonoException* exception = nullptr;
		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);

		OnPointerPressedThunk(screenPos, ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onPointerReleased(const PointerEvent& ev)
	{
		MonoException* exception = nullptr;
		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);

		OnPointerReleasedThunk(screenPos, ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptInput::onPointerDoubleClick(const PointerEvent& ev)
	{
		MonoException* exception = nullptr;
		MonoObject* screenPos = ScriptVector2I::box(ev.screenPos);

		OnPointerDoubleClickThunk(screenPos, ev.button, ev.shift, ev.control, ev.alt, ev.mouseWheelScrollAmount, &exception);

		MonoUtil::throwIfException(exception);
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

	float ScriptInput::internal_getAxisValue(UINT32 axisType, UINT32 deviceIdx)
	{
		return Input::instance().getAxisValue(axisType, deviceIdx);
	}

	void ScriptInput::internal_getPointerPosition(Vector2I* position)
	{
		*position = Input::instance().getPointerPosition();
	}
}