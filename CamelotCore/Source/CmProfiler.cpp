#include "CmProfiler.h"
#include "CmMath.h"

namespace CamelotFramework
{
	const UINT32 Profiler::NUM_SAVED_FRAMES = 200;

	Profiler::Profiler()
		:mSavedSimReports(nullptr), mCPUProfiler(nullptr), mNextSimReportIdx(0),
		mSavedCoreReports(nullptr), mNextCoreReportIdx(0)
	{
#if CM_PROFILING_ENABLED
		mCPUProfiler = cm_new<CPUProfiler>();
#endif

		mSavedSimReports = cm_newN<ProfilerReport>(NUM_SAVED_FRAMES);
		mSavedCoreReports = cm_newN<ProfilerReport>(NUM_SAVED_FRAMES);
	}

	Profiler::~Profiler()
	{
		if(mCPUProfiler != nullptr)
			cm_delete(mCPUProfiler);

		if(mSavedSimReports != nullptr)
			cm_deleteN(mSavedSimReports, NUM_SAVED_FRAMES);

		if(mSavedCoreReports != nullptr)
			cm_deleteN(mSavedCoreReports, NUM_SAVED_FRAMES);
	}

	void Profiler::update()
	{
#if CM_PROFILING_ENABLED
		mSavedSimReports[mNextSimReportIdx].cpuReport = mCPUProfiler->generateReport();

		mCPUProfiler->reset();

		mNextSimReportIdx = (mNextSimReportIdx + 1) % NUM_SAVED_FRAMES;
#endif
	}

	void Profiler::updateCore()
	{
#if CM_PROFILING_ENABLED
		CM_LOCK_MUTEX(mSync);
		mSavedCoreReports[mNextCoreReportIdx].cpuReport = mCPUProfiler->generateReport();

		mCPUProfiler->reset();

		mNextCoreReportIdx = (mNextCoreReportIdx + 1) % NUM_SAVED_FRAMES;
#endif
	}

	const ProfilerReport& Profiler::getReport(ProfiledThread thread, UINT32 idx) const
	{
		idx = Math::Clamp(idx, 0U, (UINT32)(NUM_SAVED_FRAMES - 1));

		if(thread == ProfiledThread::Core)
		{
			CM_LOCK_MUTEX(mSync);

			UINT32 reportIdx = mNextCoreReportIdx + (UINT32)((INT32)NUM_SAVED_FRAMES - ((INT32)idx + 1));
			reportIdx = (reportIdx) % NUM_SAVED_FRAMES;

			return mSavedCoreReports[reportIdx];
		}
		else
		{
			UINT32 reportIdx = mNextSimReportIdx + (UINT32)((INT32)NUM_SAVED_FRAMES - ((INT32)idx + 1));
			reportIdx = (reportIdx) % NUM_SAVED_FRAMES;

			return mSavedSimReports[reportIdx];
		}
	}

	Profiler& gProfiler()
	{
		return Profiler::instance();
	}
}