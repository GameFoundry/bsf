#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmCPUProfiler.h"

namespace CamelotFramework
{
	struct ProfilerReport
	{
		CPUProfilerReport cpuReport;
	};

	enum class ProfiledThread
	{
		Sim,
		Core
	};

	class CM_EXPORT Profiler : public Module<Profiler>
	{
	public:
		Profiler();
		~Profiler();

		/**
		 * @copydoc CPUProfiler::beginThread
		 */
		void beginThread(const String& name) 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->beginThread(name); 
#endif
		}

		/**
		 * @copydoc CPUProfiler::endThread
		 */
		void endThread() 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->endThread(); 
#endif
		}

		/**
		 * @copydoc CPUProfiler::beginSample
		 */
		void beginSample(const String& name) 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->beginSample(name); 
#endif
		}

		/**
		 * @copydoc CPUProfiler::endSample
		 */
		void endSample(const String& name) 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->endSample(name); 
#endif
		}

		/**
		 * @copydoc CPUProfiler::beginSamplePrecise
		 */
		void beginSamplePrecise(const String& name) 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->beginSamplePrecise(name); 
#endif
		}

		/**
		 * @copydoc CPUProfiler::endSamplePrecise
		 */
		void endSamplePrecise(const String& name) 
		{ 
#if CM_PROFILING_ENABLED
			mCPUProfiler->endSamplePrecise(name); 
#endif
		}

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();

		/**
		 * @brief	Called every frame from the core thread. Internal method.
		 * 			
		 * @note	Only call from core thread.
		 */
		void updateCore();

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

		CPUProfiler* mCPUProfiler;

		CM_MUTEX(mSync);
	};

	CM_EXPORT Profiler& gProfiler();
}