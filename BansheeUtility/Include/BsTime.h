#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages all time related functionality.
	 * 			
	 * @note	Sim thread only unless where specified otherwise.
	 */
	class BS_UTILITY_EXPORT Time : public Module<Time>
	{
	public:
		Time();
		~Time();

		/**
		 * @brief	Gets the time elapsed since application start.
		 * 			Only gets updated once per frame.
		 *
		 * @return	The time since application start, in seconds.
		 */
		float getTime() const { return mTimeSinceStart; }

		/**
		 * @brief	Gets the time elapsed since application start.
		 * 			Only gets updated once per frame.
		 *
		 * @return	The time since application start, in miliseconds.
		 */
		UINT64 getTimeMs() const { return mTimeSinceStartMs; }

		/**
		 * @brief	Gets the time since last frame was executed.
		 * 			Only gets updated once per frame.
		 *
		 * @return	Time since last frame was executed, in seconds.
		 */
		float getFrameDelta() const { return mFrameDelta; }

		/**
		 * @brief	Returns the sequential index of the current frame. First frame is 0.
		 *
		 * @return	The current frame.
		 *
		 * @note	Thread safe, but only counts sim thread frames.
		 */
		UINT64 getFrameIdx() const { return mCurrentFrame.load(); }

		/**
		 * @brief	Returns the precise time since application start, in microseconds.
		 * 			Unlike other time methods this is not only updated every frame,
		 * 			but will return exact time at the moment it is called.
		 * 			
		 * @note	You will generally only want to use this for performance measurements and similar.
		 * 			Use non-precise methods in majority of code as it is useful to keep the time value equal
		 * 			in all methods during a single frame.
		 *
		 * @return	Time in microseconds.
		 */
		UINT64 getTimePrecise() const;

		/**
		 * @brief	Gets the time at which the application was started, counting
		 * 			from system start.
		 *
		 * @return	The time since system to application start, in milliseconds.
		 */
		UINT64 getStartTimeMs() const { return mAppStartTime; }

		/**
		 * @brief	Called every frame. Should only be called by Application.
		 */
		void update();

		/**
		 * @brief	Multiply with time in microseconds to get a time in seconds.
		 */
		static const double MICROSEC_TO_SEC;
	private:
		float mFrameDelta; /**< Frame delta in seconds */
		float mTimeSinceStart; /**< Time since start in seconds */
		UINT64 mTimeSinceStartMs;

		UINT64 mAppStartTime; /**< Time the application started, in microseconds */
		unsigned long mLastFrameTime; /**< Time since last runOneFrame call, In microseconds */
		std::atomic<unsigned long> mCurrentFrame;

		Timer* mTimer;
	};

	BS_UTILITY_EXPORT Time& gTime();
}