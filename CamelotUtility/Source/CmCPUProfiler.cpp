#include "CmCPUProfiler.h"
#include "CmDebug.h"

#if CM_PLATFORM == CM_PLATFORM_WIN32
#include "windows.h"
#endif

namespace CamelotFramework
{
	CPUProfiler::Timer::Timer()
	{
		time = 0.0f;
	}

	void CPUProfiler::Timer::start()
	{
		startTime = getCurrentTime();
	}

	void CPUProfiler::Timer::stop()
	{
		time += getCurrentTime() - startTime;
	}

	void CPUProfiler::Timer::reset()
	{
		time = 0.0f;
	}

	inline double CPUProfiler::Timer::getCurrentTime() 
	{
		// TODO: I should be calling Platform:: performance methods instead of doing it here.
		// The only problem is that Platform belong to Core and not Utility
#if CM_PLATFORM == CM_PLATFORM_WIN32
		LARGE_INTEGER counterValue;
		QueryPerformanceCounter(&counterValue);

		LARGE_INTEGER counterFrequency;
		QueryPerformanceFrequency(&counterFrequency);

		return (double)counterValue.QuadPart / (counterFrequency.QuadPart * 0.001);
#else
		NOT IMPLEMENTED
#endif
	}

	CPUProfiler::TimerPrecise::TimerPrecise()
	{
		cycles = 0;
	}

	void CPUProfiler::TimerPrecise::start()
	{
		startCycles = getNumCycles();
	}

	void CPUProfiler::TimerPrecise::stop()
	{
		cycles += getNumCycles() - startCycles;
	}

	void CPUProfiler::TimerPrecise::reset()
	{
		cycles = 0;
	}

	inline UINT64 CPUProfiler::TimerPrecise::getNumCycles() 
	{
#if CM_COMPILER == CM_COMPILER_GNUC
		asm volatile("cpuid" : : : "%eax", "%ebx", "%ecx", "%edx" );
		UINT32 __a,__d;
		asm volatile("rdtsc" : "=a" (__a), "=d" (__d));
		return ( UINT64(__a) | UINT64(__d) << 32 );
#else
		int a[4];
		int b = 0;
		__cpuid(a, b);
		return __rdtsc();
#endif		
	}

	void CPUProfiler::ProfileData::beginSample()
	{
		timer.reset();
		timer.start();
	}

	void CPUProfiler::ProfileData::endSample()
	{
		timer.stop();
		samples.push_back(ProfileSample(timer.time));
	}

	void CPUProfiler::ProfileData::resumeLastSample()
	{
		timer.start();
		samples.erase(samples.end() - 1);
	}

	void CPUProfiler::PreciseProfileData::beginSample()
	{
		timer.reset();
		timer.start();
	}

	void CPUProfiler::PreciseProfileData::endSample()
	{
		timer.stop();
		samples.push_back(PreciseProfileSample(timer.cycles));
	}

	void CPUProfiler::PreciseProfileData::resumeLastSample()
	{
		timer.start();
		samples.erase(samples.end() - 1);
	}

	CM_THREADLOCAL CPUProfiler::ThreadInfo* CPUProfiler::ThreadInfo::activeThread = nullptr;

	CPUProfiler::ThreadInfo::ThreadInfo()
		:isActive(false), activeBlock(nullptr), rootBlock(nullptr), 
		activePreciseBlock(nullptr), rootPreciseBlock(nullptr)
	{

	}

	void CPUProfiler::ThreadInfo::begin(const String& _name)
	{
		if(isActive)
		{
			LOGWRN("Profiler::beginThread called on a thread that was already being sampled");
			return;
		}

		if(rootBlock == nullptr)
			rootBlock = getBlock();

		if(rootPreciseBlock == nullptr)
			rootPreciseBlock = getPreciseBlock();

		activeBlocks.push(rootBlock);
		activeBlock = rootBlock;

		activePreciseBlocks.push(rootPreciseBlock);
		activePreciseBlock = rootPreciseBlock;

		rootBlock->name = _name; 
		rootBlock->data.beginSample();

		rootPreciseBlock->name = _name;
		rootPreciseBlock->data.beginSample();
	}

	void CPUProfiler::ThreadInfo::end()
	{
		activePreciseBlock->data.endSample();
		activePreciseBlocks.pop();

		activeBlock->data.endSample();
		activeBlocks.pop();

		if(!isActive)
			LOGWRN("Profiler::endThread called on a thread that isn't being sampled.");

		if(activeBlocks.size() > 0)
		{
			LOGWRN("Profiler::endThread called but not all sample pairs were closed. Sampling data will not be valid.");

			while(activeBlocks.size() > 0)
			{
				ProfiledBlock* block = activeBlocks.top();
				block->data.endSample();

				activeBlocks.pop();
			}
		}

		if(activePreciseBlocks.size() > 0)
		{
			LOGWRN("Profiler::endThread called but not all sample pairs were closed. Sampling data will not be valid.");

			while(activePreciseBlocks.size() > 0)
			{
				PreciseProfiledBlock* block = activePreciseBlocks.top();
				block->data.endSample();

				activePreciseBlocks.pop();
			}
		}

		isActive = false;
		activeBlocks = Stack<ProfiledBlock*>::type();
		activeBlock = nullptr;
		activePreciseBlocks = Stack<PreciseProfiledBlock*>::type();
		activePreciseBlock = nullptr;
	}

	void CPUProfiler::ThreadInfo::reset()
	{
		if(isActive)
			end();

		if(rootBlock != nullptr)
			releaseBlock(rootBlock);

		if(rootPreciseBlock != nullptr)
			releasePreciseBlock(rootPreciseBlock);

		rootBlock = nullptr;
		rootPreciseBlock = nullptr;
	}

	CPUProfiler::ProfiledBlock* CPUProfiler::ThreadInfo::getBlock()
	{
		// TODO - Pool this, if possible using the memory allocator stuff
		// TODO - Also consider moving all samples in ThreadInfo, and also pool them (otherwise I can't pool ProfiledBlock since it will be variable size)
		return cm_new<ProfiledBlock>();
	}

	void CPUProfiler::ThreadInfo::releaseBlock(CPUProfiler::ProfiledBlock* block)
	{
		cm_delete(block);
	}

	CPUProfiler::PreciseProfiledBlock* CPUProfiler::ThreadInfo::getPreciseBlock()
	{
		// TODO - Pool this, if possible using the memory allocator stuff
		// TODO - Also consider moving all samples in ThreadInfo, and also pool them (otherwise I can't pool ProfiledBlock since it will be variable size)
		return cm_new<PreciseProfiledBlock>();
	}

	void CPUProfiler::ThreadInfo::releasePreciseBlock(CPUProfiler::PreciseProfiledBlock* block)
	{
		cm_delete(block);
	}

	CPUProfiler::ProfiledBlock::ProfiledBlock()
	{

	}

	CPUProfiler::ProfiledBlock::~ProfiledBlock()
	{
		ThreadInfo* thread = ThreadInfo::activeThread;

		for(auto& child : children)
			thread->releaseBlock(child);

		children.clear();
	}

	CPUProfiler::ProfiledBlock* CPUProfiler::ProfiledBlock::findChild(const String& name) const
	{
		for(auto& child : children)
		{
			if(child->name == name)
				return child;
		}

		return nullptr;
	}

	CPUProfiler::PreciseProfiledBlock::PreciseProfiledBlock()
	{

	}

	CPUProfiler::PreciseProfiledBlock::~PreciseProfiledBlock()
	{
		ThreadInfo* thread = ThreadInfo::activeThread;

		for(auto& child : children)
			thread->releasePreciseBlock(child);

		children.clear();
	}

	CPUProfiler::PreciseProfiledBlock* CPUProfiler::PreciseProfiledBlock::findChild(const String& name) const
	{
		for(auto& child : children)
		{
			if(child->name == name)
				return child;
		}

		return nullptr;
	}

	CPUProfiler::CPUProfiler()
		:mBasicTimerOverhead(0.0), mPreciseTimerOverhead(0), mBasicSamplingOverhead(0.0), mPreciseSamplingOverhead(0)
	{
		// TODO - We only estimate overhead on program start. It might be better to estimate it each time beginThread is called,
		// and keep separate values per thread.
		estimateTimerOverhead();
	}

	CPUProfiler::~CPUProfiler()
	{
		reset();

		CM_LOCK_MUTEX(mThreadSync);

		for(auto& threadInfo : mActiveThreads)
		{
			threadInfo->releaseBlock(threadInfo->rootBlock);
			cm_delete(threadInfo);
		}
	}

	void CPUProfiler::beginThread(const String& name)
	{
		ThreadInfo* thread = ThreadInfo::activeThread;
		if(thread == nullptr)
		{
			ThreadInfo::activeThread = cm_new<ThreadInfo>();
			thread = ThreadInfo::activeThread;

			{
				CM_LOCK_MUTEX(mThreadSync);

				mActiveThreads.push_back(thread);
			}
		}

		thread->begin(name);
	}

	void CPUProfiler::endThread()
	{
		// I don't do a nullcheck where on purpose, so endSample can be called ASAP
		ThreadInfo::activeThread->end();
	}

	void CPUProfiler::beginSample(const String& name)
	{
		ThreadInfo* thread = ThreadInfo::activeThread;
		if(thread == nullptr || !thread->isActive)
			beginThread("Unknown");

		ProfiledBlock* parent = thread->activeBlock;
		ProfiledBlock* block = nullptr;
		
		parent->findChild(name);

		if(block == nullptr)
		{
			block = thread->getBlock();
			block->name = name;

			parent->children.push_back(block);
			thread->activePreciseBlock->basicChildren.push_back(block);

			thread->activeBlocks.push(block);
			thread->activeBlock = block;
		}

		block->data.beginSample();
	}

	void CPUProfiler::endSample(const String& name)
	{
		ThreadInfo* thread = ThreadInfo::activeThread;
		ProfiledBlock* block = thread->activeBlock;
		block->data.endSample();

		if(block->name != name)
		{
			LOGWRN("Mismatched Profiler::endSample. Was expecting \"" + block->name + "\" but got \"" + name + "\". Sampling data will not be valid.");

			block->data.resumeLastSample();

			return;
		}

		thread->activeBlocks.pop();
		thread->activeBlock = thread->activeBlocks.top();
	}

	void CPUProfiler::beginSamplePrecise(const String& name)
	{
		// Note: There is a (small) possibility a context switch will happen during this measurement in which case result will be skewed. 
		// Increasing thread priority might help. This is generally only a problem with code that executes a long time (10-15+ ms - depending on OS quant length)
		
		ThreadInfo* thread = ThreadInfo::activeThread;
		if(thread == nullptr || !thread->isActive)
			beginThread("Unknown");

		PreciseProfiledBlock* parent = thread->activePreciseBlock;
		PreciseProfiledBlock* block = nullptr;
		
		parent->findChild(name);

		if(block == nullptr)
		{
			block = thread->getPreciseBlock();
			block->name = name;

			parent->children.push_back(block);
			thread->activeBlock->preciseChildren.push_back(block);

			thread->activePreciseBlocks.push(block);
			thread->activePreciseBlock = block;
		}

		block->data.beginSample();
	}

	void CPUProfiler::endSamplePrecise(const String& name)
	{
		ThreadInfo* thread = ThreadInfo::activeThread;
		PreciseProfiledBlock* block = thread->activePreciseBlock;
		block->data.endSample();

		if(block->name != name)
		{
			LOGWRN("Mismatched Profiler::endSamplePrecise. Was expecting \"" + block->name + "\" but got \"" + name + "\". Sampling data will not be valid.");

			block->data.resumeLastSample();

			return;
		}

		thread->activePreciseBlocks.pop();
		thread->activePreciseBlock = thread->activePreciseBlocks.top();
	}

	void CPUProfiler::update()
	{
		// TODO: Keep track of FPS
	}

	void CPUProfiler::reset()
	{
		ThreadInfo* thread = ThreadInfo::activeThread;

		if(thread != nullptr)
			thread->reset();
	}

	CPUProfilerReport CPUProfiler::generateReport()
	{
		CPUProfilerReport report;

		ThreadInfo* thread = ThreadInfo::activeThread;
		if(thread == nullptr)
			return report;

		if(thread->isActive)
			thread->end();

		if(thread->rootBlock != nullptr)
		{
			// Fill up flatHierarchy array in a way so we always process
			// children before parents
			Stack<ProfiledBlock*>::type todo;
			Vector<ProfiledBlock*>::type flatHierarchy;
			Vector<CPUProfilerBasicSamplingEntry*>::type flatResultHierarchy;

			todo.push(thread->rootBlock);
			flatHierarchy.push_back(thread->rootBlock);
			flatResultHierarchy.push_back(&report.mBasicSamplingRootEntry);

			while(!todo.empty())
			{
				ProfiledBlock* curBlock = todo.top();
				todo.pop();

				CPUProfilerBasicSamplingEntry* parentEntry = flatResultHierarchy.back();
				for(auto& child : curBlock->children)
				{
					todo.push(child);
					flatHierarchy.push_back(child);

					parentEntry->childEntries.push_back(CPUProfilerBasicSamplingEntry());
					flatResultHierarchy.push_back(&parentEntry->childEntries.back());					
				}
			}

			auto& iter = flatHierarchy.rbegin();
			auto& iterSample = flatResultHierarchy.rbegin();

			for(; iter != flatHierarchy.rend(); ++iter, ++iterSample)
			{
				ProfiledBlock* curBlock = *iter;
				CPUProfilerBasicSamplingEntry* entry = *iterSample;

				entry->data.name = curBlock->name;

				entry->data.totalTimeMs = 0.0;
				entry->data.maxTimeMs = 0.0;
				for(auto& sample : curBlock->data.samples)
				{
					entry->data.totalTimeMs += sample.time;
					entry->data.maxTimeMs = std::max(entry->data.maxTimeMs, sample.time);
				}

				entry->data.numCalls = (UINT32)curBlock->data.samples.size();
				entry->data.avgTimeMs = entry->data.totalTimeMs / entry->data.numCalls;

				UINT32 childIdx = 0;
				double totalChildTime = 0.0;
				for(auto& child : curBlock->children)
				{
					totalChildTime += entry->childEntries[childIdx].data.totalTimeMs;
					entry->childEntries[childIdx].data.pctOfParent = entry->childEntries[childIdx].data.totalTimeMs / entry->data.totalTimeMs;

					entry->data.estimatedOverheadMs += entry->childEntries[childIdx].data.estimatedOverheadMs + mBasicSamplingOverhead;

					childIdx++;
				}

				entry->data.totalSelfTimeMs = entry->data.totalTimeMs - totalChildTime;
				entry->data.avgSelfTimeMs = entry->data.totalSelfTimeMs / entry->data.numCalls;

				entry->data.estimatedSelfOverheadMs = mBasicTimerOverhead;
			}
		}

		if(thread->rootPreciseBlock != nullptr)
		{
			// Fill up flatHierarchy array in a way so we always process
			// children before parents
			Stack<PreciseProfiledBlock*>::type todo;
			Vector<PreciseProfiledBlock*>::type flatHierarchy;
			Vector<CPUProfilerPreciseSamplingEntry*>::type flatResultHierarchy;

			todo.push(thread->rootPreciseBlock);
			flatHierarchy.push_back(thread->rootPreciseBlock);
			flatResultHierarchy.push_back(&report.mPreciseSamplingRootEntry);

			while(!todo.empty())
			{
				PreciseProfiledBlock* curBlock = todo.top();
				todo.pop();

				CPUProfilerPreciseSamplingEntry* parentEntry = flatResultHierarchy.back();
				for(auto& child : curBlock->children)
				{
					todo.push(child);
					flatHierarchy.push_back(child);

					parentEntry->childEntries.push_back(CPUProfilerPreciseSamplingEntry());
					flatResultHierarchy.push_back(&parentEntry->childEntries.back());					
				}
			}

			auto& iter = flatHierarchy.rbegin();
			auto& iterSample = flatResultHierarchy.rbegin();

			for(; iter != flatHierarchy.rend(); ++iter, ++iterSample)
			{
				PreciseProfiledBlock* curBlock = *iter;
				CPUProfilerPreciseSamplingEntry* entry = *iterSample;

				entry->data.name = curBlock->name;

				entry->data.totalCycles = 0;
				entry->data.maxCycles = 0;
				for(auto& sample : curBlock->data.samples)
				{
					entry->data.totalCycles += sample.cycles;
					entry->data.maxCycles = std::max(entry->data.maxCycles, sample.cycles);
				}

				entry->data.numCalls = (UINT32)curBlock->data.samples.size();
				entry->data.avgCycles = entry->data.avgCycles / entry->data.numCalls;

				UINT32 childIdx = 0;
				UINT64 totalChildCycles = 0;
				for(auto& child : curBlock->children)
				{
					totalChildCycles += entry->childEntries[childIdx].data.totalCycles;
					entry->childEntries[childIdx].data.pctOfParent = entry->childEntries[childIdx].data.totalCycles / (float)entry->data.totalCycles;

					entry->data.estimatedOverhead += entry->childEntries[childIdx].data.estimatedOverhead + mPreciseSamplingOverhead;

					childIdx++;
				}

				entry->data.totalSelfCycles = entry->data.totalCycles - totalChildCycles;
				entry->data.avgSelfCycles = entry->data.totalSelfCycles / entry->data.numCalls;

				entry->data.estimatedSelfOverhead = mPreciseTimerOverhead;
			}
		}
	}

	void CPUProfiler::estimateTimerOverhead()
	{
		// Get an idea of how long timer calls and RDTSC takes
		const UINT32 reps = 1000, sampleReps = 100;

		mBasicTimerOverhead = 1000000.0;
		mPreciseTimerOverhead = 1000000;
		for (UINT32 tries = 0; tries < 20; tries++) 
		{
			Timer timer;
			for (UINT32 i = 0; i < reps; i++) 
			{
				timer.start();
				timer.stop();
			}

			double avgTime = double(timer.time)/double(reps);
			if (avgTime < mBasicTimerOverhead)
				mBasicTimerOverhead = avgTime;

			TimerPrecise timerPrecise;
			for (UINT32 i = 0; i < reps; i++) 
			{
				timerPrecise.start();
				timerPrecise.stop();
			}

			UINT64 avgCycles = timerPrecise.cycles/reps;
			if (avgCycles < mPreciseTimerOverhead)
				mPreciseTimerOverhead = avgCycles;
		}

		for (UINT32 tries = 0; tries < 20; tries++) 
		{
			Timer timer;
			timer.start();
			beginThread("Main");

			// Two different cases that can effect performance, one where
			// sample already exists and other where new one needs to be created
			for (UINT32 i = 0; i < sampleReps; i++) 
			{
				beginSample("TestAvg1");
				endSample("TestAvg1");
				beginSample("TestAvg2");
				endSample("TestAvg2");
				beginSample("TestAvg3");
				endSample("TestAvg3");
				beginSample("TestAvg4");
				endSample("TestAvg4");
				beginSample("TestAvg5");
				endSample("TestAvg5");
				beginSample("TestAvg6");
				endSample("TestAvg6");
				beginSample("TestAvg7");
				endSample("TestAvg7");
				beginSample("TestAvg8");
				endSample("TestAvg8");
				beginSample("TestAvg9");
				endSample("TestAvg9");
				beginSample("TestAvg10");
				endSample("TestAvg10");
			}

			for (UINT32 i = 0; i < sampleReps * 5; i++) 
			{
				beginSample("Test#" + toString(i));
				endSample("Test#" + toString(i));
			}

			endThread();
			timer.stop();

			reset();

			double avgTime = double(timer.time)/double(sampleReps * 10 + sampleReps * 5);
			if (avgTime < mBasicSamplingOverhead)
				mBasicSamplingOverhead = avgTime;

			TimerPrecise timerPrecise;
			timerPrecise.start();
			beginThread("Main");

			// Two different cases that can effect performance, one where
			// sample already exists and other where new one needs to be created
			for (UINT32 i = 0; i < sampleReps; i++) 
			{
				beginSamplePrecise("TestAvg1");
				endSamplePrecise("TestAvg1");
				beginSamplePrecise("TestAvg2");
				endSamplePrecise("TestAvg2");
				beginSamplePrecise("TestAvg3");
				endSamplePrecise("TestAvg3");
				beginSamplePrecise("TestAvg4");
				endSamplePrecise("TestAvg4");
				beginSamplePrecise("TestAvg5");
				endSamplePrecise("TestAvg5");
				beginSamplePrecise("TestAvg6");
				endSamplePrecise("TestAvg6");
				beginSamplePrecise("TestAvg7");
				endSamplePrecise("TestAvg7");
				beginSamplePrecise("TestAvg8");
				endSamplePrecise("TestAvg8");
				beginSamplePrecise("TestAvg9");
				endSamplePrecise("TestAvg9");
				beginSamplePrecise("TestAvg10");
				endSamplePrecise("TestAvg10");
			}

			for (UINT32 i = 0; i < sampleReps * 5; i++) 
			{
				beginSamplePrecise("Test#" + toString(i));
				endSamplePrecise("Test#" + toString(i));
			}

			endThread();
			timerPrecise.stop();

			reset();

			UINT64 avgCycles = timerPrecise.cycles/(sampleReps * 10 + sampleReps * 5);
			if (avgCycles < mPreciseSamplingOverhead)
				mPreciseSamplingOverhead = avgCycles;
		}
	}

	CPUProfilerBasicSamplingEntry::Data::Data()
		:numCalls(0), avgTimeMs(0.0), maxTimeMs(0.0), totalTimeMs(0.0),
		avgSelfTimeMs(0.0), totalSelfTimeMs(0.0), estimatedSelfOverheadMs(0.0),
		estimatedOverheadMs(0.0), pctOfParent(1.0f)
	{ }

	CPUProfilerPreciseSamplingEntry::Data::Data()
		:numCalls(0), avgCycles(0), maxCycles(0), totalCycles(0),
		avgSelfCycles(0), totalSelfCycles(0), estimatedSelfOverhead(0),
		estimatedOverhead(0), pctOfParent(1.0f)
	{ }
}