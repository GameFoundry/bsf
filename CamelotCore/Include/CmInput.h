#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRect.h"
#include "CmOSInputHandler.h"
#include "CmRawInputHandler.h"
#include "CmInputFwd.h"

namespace CamelotFramework
{
	class CM_EXPORT Input : public Module<Input>
	{
	public:
		Input();
		~Input();

		boost::signal<void(const ButtonEvent&)> onButtonDown;
		boost::signal<void(const ButtonEvent&)> onButtonUp;
		boost::signal<void(const TextInputEvent&)> onCharInput;

		boost::signal<void(const MouseEvent&)> onMouseMoved;

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

		bool isButtonDown(ButtonCode keyCode) const;

		Int2 getMousePosition() const { return mMouseAbsPos; }

		// Thread safe. Will only be processed on next "update".
		void simulateButtonDown(ButtonCode code);

		// Thread safe. Will only be processed on next "update".
		void simulateButtonUp(ButtonCode code);

	private:
		std::shared_ptr<RawInputHandler> mRawInputHandler;
		std::shared_ptr<OSInputHandler> mOSInputHandler;

		CM_MUTEX(mSimulatedInputMutex);
		Vector<ButtonCode>::type mSimulatedButtonUp;
		Vector<ButtonCode>::type mSimulatedButtonDown;

		float mSmoothHorizontalAxis;
		float mSmoothVerticalAxis;

		float* mHorizontalHistoryBuffer;
		float* mVerticalHistoryBuffer;
		float* mTimesHistoryBuffer;
		int	mCurrentBufferIdx;

		Int2 mMouseLastRel;
		Int2 mMouseAbsPos;

		RawAxisState mAxes[RawInputAxis::Count];
		bool mKeyState[BC_Count];

		void buttonDown(ButtonCode code);
		void buttonUp(ButtonCode code);

		void charInput(UINT32 chr);

		/**
		 * @brief	Raw mouse/joystick axis input.
		 */
		void axisMoved(const RawAxisState& state, RawInputAxis axis);

		/**
		 * @brief	Mouse movement as OS reports it. Used for screen cursor position.
		 */
		void mouseMoved(const Int2& screenPos);
		
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