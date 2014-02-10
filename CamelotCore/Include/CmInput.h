#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRectI.h"
#include "CmOSInputHandler.h"
#include "CmRawInputHandler.h"
#include "CmInputFwd.h"

namespace CamelotFramework
{
	class CM_EXPORT Input : public Module<Input>
	{
		enum class ButtonState
		{
			Off,
			On,
			ToggledOn,
			ToggledOff
		};

	public:
		Input();
		~Input();

		boost::signal<void(const ButtonEvent&)> onButtonDown;
		boost::signal<void(const ButtonEvent&)> onButtonUp;
		boost::signal<void(const TextInputEvent&)> onCharInput;

		boost::signal<void(const PositionalInputEvent&)> onCursorMoved;
		boost::signal<void(const PositionalInputEvent&)> onCursorPressed;
		boost::signal<void(const PositionalInputEvent&)> onCursorReleased;
		boost::signal<void(const PositionalInputEvent&)> onDoubleClick;

		boost::signal<void(InputCommandType)> onInputCommand;

		void registerRawInputHandler(std::shared_ptr<RawInputHandler> inputHandler);

		/**
		 * @brief	Called every frame. Dispatches any callbacks resulting from input by the user. Should only be called by Application.
		 */
		void update();

		/**
		 * @brief	Returns smoothed mouse/joystick input in the horizontal axis.
		 *
		 * @return	The horizontal axis value ranging [-1.0f, 1.0f].
		 */
		float getHorizontalAxis() const;

		/**
		 * @brief	Returns smoothed mouse/joystick input in the vertical axis.
		 *
		 * @return	The vertical axis value ranging [-1.0f, 1.0f].
		 */
		float getVerticalAxis() const;

		bool isButtonHeld(ButtonCode keyCode) const;
		bool isButtonUp(ButtonCode keyCode) const;
		bool isButtonDown(ButtonCode keyCode) const;

		Vector2I getCursorPosition() const { return mMouseAbsPos; }
	private:
		std::shared_ptr<RawInputHandler> mRawInputHandler;
		std::shared_ptr<OSInputHandler> mOSInputHandler;

		float mSmoothHorizontalAxis;
		float mSmoothVerticalAxis;

		float* mHorizontalHistoryBuffer;
		float* mVerticalHistoryBuffer;
		float* mTimesHistoryBuffer;
		int	mCurrentBufferIdx;

		Vector2I mMouseLastRel;
		Vector2I mMouseAbsPos;

		RawAxisState mAxes[RawInputAxis::Count];
		ButtonState mKeyState[BC_Count];

		void buttonDown(ButtonCode code, float timestamp);
		void buttonUp(ButtonCode code, float timestamp);

		void charInput(UINT32 chr);

		/**
		 * @brief	Raw mouse/joystick axis input.
		 */
		void axisMoved(const RawAxisState& state, RawInputAxis axis);

		/**
		 * @brief	Cursor movement as OS reports it. Used for screen cursor position.
		 */
		void cursorMoved(const PositionalInputEvent& event);

		/**
		 * @brief	Cursor button presses as OS reports it. 
		 */
		void cursorPressed(const PositionalInputEvent& event);

		/**
		 * @brief	Cursor button releases as OS reports it.
		 */
		void cursorReleased(const PositionalInputEvent& event);
		
		/**
		 * @brief	Cursor button releases as OS reports it.
		 */
		void cursorDoubleClick(const PositionalInputEvent& event);

		/**
		 * @brief	Input commands as OS reports them.
		 */
		void inputCommandEntered(InputCommandType commandType);

		/**
		 * @brief	Updates the axis input values that need smoothing.
		 */
		void updateSmoothInput();

		/**
		 * @brief	Called when window in focus changes, as reported by the OS.
		 */
		void inputWindowChanged(RenderWindow& win);

		/************************************************************************/
		/* 								STATICS		                      		*/
		/************************************************************************/
		static const int HISTORY_BUFFER_SIZE; // Size of buffer used for input smoothing
		static const float WEIGHT_MODIFIER;
	};

	CM_EXPORT Input& gInput();
}