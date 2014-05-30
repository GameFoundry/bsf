#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsProfilerCPU.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains data about a profiling session.
	 */
	struct ProfilerReport
	{
		CPUProfilerReport cpuReport;
	};

	/**
	 * @brief	Type of thread used by the profiler.
	 */
	enum class ProfiledThread
	{
		Sim,
		Core
	};

	/**
	 * @brief	Tracks CPU profiling information with each frame for sim and core threads.
	 *
	 * @note	Sim thread only unless specified otherwise.
	 */
	class BS_CORE_EXPORT ProfilingManager : public Module<ProfilingManager>
	{
	public:
		ProfilingManager();
		~ProfilingManager();

		/**
		 * @brief	Called every frame.
		 *
		 * @note	Internal method.
		 */
		void _update();

		/**
		 * @brief	Called every frame from the core thread.
		 * 			
		 * @note	Internal method. Core thread only.
		 */
		void _updateCore();

		/**
		 * @brief	Returns a profiler report for the specified frame, for the specified thread.
		 *
		 * @param	Profiler report index, ranging [0, NUM_SAVED_FRAMES]. 0 always returns the latest
		 * 					 report. Increasing indexes return reports for older and older frames. Out of range
		 * 					 indexes will be clamped.
		 *
		 * @note	Profiler reports get updated every frame. Oldest reports that no longer fit in the saved reports buffer
		 * 			are discarded.
		 */
		const ProfilerReport& getReport(ProfiledThread thread, UINT32 idx = 0) const;

	private:
		static const UINT32 NUM_SAVED_FRAMES;
		ProfilerReport* mSavedSimReports;
		UINT32 mNextSimReportIdx;

		ProfilerReport* mSavedCoreReports;
		UINT32 mNextCoreReportIdx;

		BS_MUTEX(mSync);
	};

	/**
	 * @brief	Quick way to access the profiler.
	 */
	BS_CORE_EXPORT ProfilingManager& gProfiler();
}