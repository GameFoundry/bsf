#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRect2I.h"
#include "BsOSInputHandler.h"
#include "BsRawInputHandler.h"
#include "BsInputFwd.h"

namespace BansheeEngine
{
	/**
	 * @brief	Primary module used for dealing with input. Allows you to receieve 
	 *			and query raw or OS input for mouse/keyboard/gamepad.
	 *
	 *			All inputs are received through an input handler, which can be overriden to 
	 *			provide custom input functionality.
	 */
	class BS_CORE_EXPORT Input : public Module<Input>
	{
		/**
		 * @brief	Possible button states
		 */
		enum class ButtonState
		{
			Off, /**< Button is not being pressed. */
			On, /**< Button is being pressed. */
			ToggledOn, /**< Button has been pressed this frame. */
			ToggledOff /**< Button has been released this frame. */
		};

		/**
		 * @brief	Contains axis and device data per device
		 */
		struct DeviceData
		{
			DeviceData();

			Vector<RawAxisState> axes;
			ButtonState keyStates[BC_Count];
		};

	public:
		Input();
		~Input();

		/**
		 * @brief	Triggered whenever a button is first pressed.
		 */
		Event<void(const ButtonEvent&)> onButtonDown;

		/**
		 * @brief	Triggered whenever a button is first released.
		 */
		Event<void(const ButtonEvent&)> onButtonUp;

		/**
		 * @brief	Triggered whenever user inputs a text character. 
		 */
		Event<void(const TextInputEvent&)> onCharInput;

		/**
		 * @brief	Triggers when some pointing device (mouse cursor, touch) moves.
		 */
		Event<void(const PointerEvent&)> onPointerMoved;

		/**
		 * @brief	Triggers when some pointing device (mouse cursor, touch) button is pressed.
		 */
		Event<void(const PointerEvent&)> onPointerPressed;

		/**
		 * @brief	Triggers when some pointing device (mouse cursor, touch) button is released.
		 */
		Event<void(const PointerEvent&)> onPointerReleased;

		/**
		 * @brief	Triggers when some pointing device (mouse cursor, touch) button is double clicked.
		 */
		Event<void(const PointerEvent&)> onPointerDoubleClick;

		// TODO Low priority: Remove this, I can emulate it using virtual input
		/**
		 * @brief	Triggers on special input commands.
		 */
		Event<void(InputCommandType)> onInputCommand;

		/**
		 * @brief	Registers a new input handler. Replaces any previous input handler.
		 *
		 * @note	Internal method.
		 */
		void _registerRawInputHandler(std::shared_ptr<RawInputHandler> inputHandler);

		/**
		 * @brief	Called every frame. Dispatches any callbacks resulting from input by the user.
		 *
		 * @note	Internal method.
		 */
		void _update();

		/**
		 * @brief	Returns value of the specified input axis. Normally in range [-1.0, 1.0] but can be outside
		 *			the range for devices with unbound axes (e.g. mouse).
		 *
		 * @param	type		Type of axis to query. Usually a type from InputAxis but can be a custom value.
		 * @param	deviceIdx	Index of the device in case more than one is hooked up (0 - primary).
		 */
		float getAxisValue(UINT32 type, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Query if the provided button is currently being held (this frame or previous frames).
		 *
		 * @param	keyCode		Code of the button to query.
		 * @param	deviceIdx	Device to query the button on (0 - primary).
		 */
		bool isButtonHeld(ButtonCode keyCode, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Query if the provided button is currently being released (one true for one frame).
		 *
		 * @param	keyCode		Code of the button to query.
		 * @param	deviceIdx	Device to query the button on (0 - primary).
		 */
		bool isButtonUp(ButtonCode keyCode, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Query if the provided button is currently being pressed (one true for one frame).
		 *
		 * @param	keyCode		Code of the button to query.
		 * @param	deviceIdx	Device to query the button on (0 - primary).
		 */
		bool isButtonDown(ButtonCode keyCode, UINT32 deviceIdx = 0) const;

		/**
		 * @brief	Returns positions of the pointer (e.g. mouse cursor) relative to the screen.
		 */
		Vector2I getPointerPosition() const;

		/**
		 * @brief	Returns difference between last and current pointer position.
		 */
		Vector2I getPointerDelta() const { return mPointerDelta; }

		/**
		 * @brief	Query if the provided pointer button is currently 
		 *			being held (this frame or previous frames).
		 *
		 * @param	pointerButton		Code of the button to query.
		 */
		bool isPointerButtonHeld(PointerEventButton pointerButton) const;

		/**
		 * @brief	Query if the provided pointer button is currently 
		 *			being released (one true for one frame).
		 *
		 * @param	pointerButton		Code of the button to query.
		 */
		bool isPointerButtonUp(PointerEventButton pointerButton) const;

		/**
		 * @brief	Query if the provided pointer button is currently 
		 *			being pressed (one true for one frame).
		 *
		 * @param	pointerButton		Code of the button to query.
		 */
		bool isPointerButtonDown(PointerEventButton pointerButton) const;

		/**
		 * @brief	Enables or disables mouse smoothing. Smoothing makes the changes to
		 *			mouse axes more gradual.
		 */
		void setMouseSmoothing(bool enabled);

	private:
		/**
		 * @brief	Triggered by input handler when a button is pressed.
		 */
		void buttonDown(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

		/**
		 * @brief	Triggered by input handler when a button is released.
		 */
		void buttonUp(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp);

		/**
		 * @brief	Triggered by input handler when a single character is input.
		 */
		void charInput(UINT32 chr);

		/**
		 * @brief	Triggered by input handler when a mouse/joystick axis is moved.
		 */
		void axisMoved(UINT32 deviceIdx, const RawAxisState& state, UINT32 axis);

		/**
		 * @brief	Cursor movement as OS reports it. Used for screen cursor position.
		 */
		void cursorMoved(const PointerEvent& event);

		/**
		 * @brief	Cursor button presses as OS reports it. 
		 */
		void cursorPressed(const PointerEvent& event);

		/**
		 * @brief	Cursor button releases as OS reports it.
		 */
		void cursorReleased(const PointerEvent& event);
		
		/**
		 * @brief	Cursor button releases as OS reports it.
		 */
		void cursorDoubleClick(const PointerEvent& event);

		/**
		 * @brief	Input commands as OS reports them.
		 */
		void inputCommandEntered(InputCommandType commandType);

		/**
		 * @brief	Called when window in focus changes, as reported by the OS.
		 */
		void inputWindowChanged(RenderWindow& win);

	private:
		std::shared_ptr<RawInputHandler> mRawInputHandler;
		std::shared_ptr<OSInputHandler> mOSInputHandler;

		Vector<DeviceData> mDevices;
		Vector2I mPointerPosition;
		Vector2I mPointerDelta;
		ButtonState mPointerButtonStates[3];
		bool mLastPositionSet;

		/************************************************************************/
		/* 								STATICS		                      		*/
		/************************************************************************/
		static const int HISTORY_BUFFER_SIZE; // Size of buffer used for input smoothing
		static const float WEIGHT_MODIFIER;
	};

	/**
	 * @copydoc	Input
	 */
	BS_CORE_EXPORT Input& gInput();
}