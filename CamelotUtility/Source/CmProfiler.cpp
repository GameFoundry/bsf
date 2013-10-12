#include "CmProfiler.h"

namespace CamelotFramework
{
	void Profiler::beginSample(const String& name)
	{

	}

	Profiler::Data Profiler::endSample(const String& name)
	{
		return Profiler::Data();
	}

	void Profiler::beginSamplePrecise(const String& name)
	{

	}

	Profiler::PreciseData Profiler::endSamplePrecise(const String& name)
	{
		return Profiler::PreciseData();
	}

	void Profiler::update()
	{

	}
}