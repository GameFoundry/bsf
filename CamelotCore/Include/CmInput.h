#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRect.h"
#include "CmInputHandler.h"

namespace CamelotEngine
{
	class CM_EXPORT Input : public Module<Input>
	{
	public:
		Input();
		~Input();

		boost::signal<void(KeyCode)> onKeyDown;
		boost::signal<void(KeyCode)> onKeyUp;

		boost::signal<void(const MouseEvent&)> onMouseMoved;
		boost::signal<void(const MouseEvent&, MouseButton)> onMouseDown;
		boost::signal<void(const MouseEvent&, MouseButton)> onMouseUp;


		void initClipRect(Rect& clipRect);
		void registerInputHandler(InputHandlerPtr inputHandler);

		/**
		 * @brief	Called every frame. Should only be called by Application.
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

	private:
		InputHandlerPtr mInputHandler;

		float mSmoothHorizontalAxis;
		float mSmoothVerticalAxis;

		float* mHorizontalHistoryBuffer;
		float* mVerticalHistoryBuffer;
		float* mTimesHistoryBuffer;
		int	mCurrentBufferIdx;

		Point mMouseLastRel;
		Rect mClipRect;
		bool mUsingClipRect;

		void keyDown(KeyCode keyCode);
		void keyUp(KeyCode keyCode);

		void mouseMoved(const MouseEvent& event);
		void mouseDown(const MouseEvent& event, MouseButton buttonID);
		void mouseUp(const MouseEvent& event, MouseButton buttonID);

		/**
		 * @brief	Updates the input values that need smoothing.
		 */
		void updateSmoothInput();

		/************************************************************************/
		/* 								STATICS		                      		*/
		/************************************************************************/
		static const int HISTORY_BUFFER_SIZE; // Size of buffer used for input smoothing
		static const float WEIGHT_MODIFIER;
	};

	CM_EXPORT Input& gInput();
}