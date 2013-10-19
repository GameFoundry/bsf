#include "CmProfiler.h"
#include "CmMath.h"

namespace CamelotFramework
{
	const UINT32 Profiler::NUM_SAVED_FRAMES = 200;

	Profiler::Profiler()
		:mSavedReports(nullptr), mCPUProfiler(nullptr), mNextReportIdx(0)
	{
#if CM_PROFILING_ENABLED
		mCPUProfiler = cm_new<CPUProfiler>();
#endif

		mSavedReports = cm_newN<ProfilerReport>(NUM_SAVED_FRAMES);
	}

	Profiler::~Profiler()
	{
		if(mCPUProfiler != nullptr)
			cm_delete(mCPUProfiler);

		if(mSavedReports != nullptr)
			cm_deleteN(mSavedReports, NUM_SAVED_FRAMES);
	}

	void Profiler::update()
	{
#if CM_PROFILING_ENABLED
		mSavedReports[mNextReportIdx].cpuReport = mCPUProfiler->generateReport();

		mCPUProfiler->reset();

		mNextReportIdx = (mNextReportIdx + 1) % NUM_SAVED_FRAMES;
#endif
	}

	const ProfilerReport& Profiler::getReport(UINT32 idx) const
	{
		idx = Math::Clamp(idx, 0U, (UINT32)(NUM_SAVED_FRAMES - 1));

		return mSavedReports[idx];
	}

	Profiler& gProfiler()
	{
		return Profiler::instance();
	}
}