#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Manages all time related functionality
	 */
	class CM_UTILITY_EXPORT Time : public Module<Time>
	{
	public:
		Time();
		~Time();

		/**
		 * @brief	Gets the time elapsed since application start.
		 *
		 * @return	The time since application start, in seconds.
		 */
		float getTimeSinceApplicationStart() { return mTimeSinceStart; }

		/**
		 * @brief	Gets the time since last frame was executed.
		 *
		 * @return	Time since last frame was executed, in seconds.
		 */
		float getFrameDelta() { return mFrameDelta; }

		/**
		 * @brief	Returns the number of the current frame. First frame is 0.
		 *
		 * @return	The current frame.
		 */
		unsigned long getCurrentFrameNumber() { return mCurrentFrame; }

		/**
		 * @brief	Called when the application is first started. Should only be called by Application.
		 */
		void init();

		/**
		 * @brief	Called every frame. Should only be called by Application.
		 */
		void update();

	private:
		float mFrameDelta; // Frame delta in seconds
		float mTimeSinceStart; // Time since start in seconds

		unsigned long mAppStartTime; // Time the application started, in microseconds
		unsigned long mLastFrameTime; // Time since last runOneFrame call, In microseconds
		unsigned long mCurrentFrame;

		Timer* mTimer;

		static const double MICROSEC_TO_SEC;
	};

	CM_UTILITY_EXPORT Time& gTime();
}