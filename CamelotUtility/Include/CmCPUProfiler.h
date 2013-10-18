#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CPUProfilerReport;

	// TODO: Add #defines for all profiler methods so we can easily remove them from final version

	/**
	 * @brief	Provides various performance measuring methods
	 * 			
	 * @note	This class is thread safe. Matching begin*\end* calls
	 * 			must belong to the same thread though.
	 */
	class CM_UTILITY_EXPORT CPUProfiler
	{
		class Timer
		{
		public:
			Timer();

			void start();
			void stop();
			void reset();

			double time;
		private:
			double startTime;

			static inline double getCurrentTime();
		};

		class TimerPrecise
		{
		public:
			TimerPrecise();

			void start();
			void stop();
			void reset();

			UINT64 cycles;
		private:
			UINT64 startCycles;

			static inline UINT64 getNumCycles();
		};

		struct ProfileSample
		{
			ProfileSample(double _time)
				:time(_time)
			{ }

			double time;
		};

		struct PreciseProfileSample
		{
			PreciseProfileSample(UINT64 _cycles)
				:cycles(_cycles)
			{ }

			UINT64 cycles;
		};

		struct ProfileData
		{
			Vector<ProfileSample>::type samples;
			Timer timer;

			void beginSample();
			void endSample();
			void resumeLastSample();
		};

		struct PreciseProfileData
		{
			// TODO - Add cache misses, branch mispredictions, retired instructions vs. optimal number of cycles (RDPMC instruction on Intel)

			Vector<PreciseProfileSample>::type samples;
			TimerPrecise timer;

			void beginSample();
			void endSample();
			void resumeLastSample();
		};

		struct PreciseProfiledBlock;
		struct ProfiledBlock;

		struct ProfiledBlock
		{
			ProfiledBlock();
			~ProfiledBlock();

			String name;
			
			ProfileData basic;
			PreciseProfileData precise;

			Vector<ProfiledBlock*>::type children;

			ProfiledBlock* findChild(const String& name) const;
		};

		enum class ActiveSamplingType
		{
			Basic,
			Precise
		};

		struct ActiveBlock
		{
			ActiveBlock()
				:type(ActiveSamplingType::Basic), block(nullptr)
			{ }

			ActiveBlock(ActiveSamplingType _type, ProfiledBlock* _block)
				:type(_type), block(_block)
			{ }

			ActiveSamplingType type;
			ProfiledBlock* block;
		};

		struct ThreadInfo
		{
			ThreadInfo();

			static CM_THREADLOCAL ThreadInfo* activeThread;
			bool isActive;

			ProfiledBlock* rootBlock;

			Stack<ActiveBlock>::type activeBlocks;
			ActiveBlock activeBlock;

			void begin(const String& _name);
			void end();
			void reset();

			ProfiledBlock* getBlock();
			void releaseBlock(ProfiledBlock* block);
		};

	public:
		CPUProfiler();
		~CPUProfiler();

		/**
		 * @brief	Registers a new thread we will be doing sampling in. This needs to be called before any beginSample*\endSample* calls
		 * 			are made in that thread.
		 *
		 * @param	name	Name that will allow you to more easily identify the thread.
		 */
		void beginThread(const String& name);

		/**
		 * @brief	Ends sampling for the current thread. No beginSample*\endSample* calls after this point.
		 */
		void endThread();

		/**
		 * @brief	Begins sample measurement. Must be followed by endSample. 
		 *
		 * @param	name	Unique name for the sample you can later use to find the sampling data.
		 */
		void beginSample(const String& name);

		/**
		 * @brief	Ends sample measurement and returns measured data.
		 *
		 * @param	name	Unique name for the sample. 
		 * 					
		 * @note	Unique name is primarily needed to more easily identify mismatched
		 * 			begin/end sample pairs. Otherwise the name in beginSample would be enough.
		 */
		void endSample(const String& name);

		/**
		 * @brief	Begins sample measurement. Must be followed by endSample. 
		 *
		 * @param	name	Unique name for the sample you can later use to find the sampling data.
		 * 					
		 * @note	This method uses very precise CPU counters to determine variety of data not
		 * 			provided by standard beginSample. However due to the way these counters work you should
		 * 			not use this method for larger parts of code. It does not consider context switches so if the OS
		 * 			decides to switch context between measurements you will get invalid data.
		 */
		void beginSamplePrecise(const String& name);

		/**
		 * @brief	Ends precise sample measurement and returns measured data.
		 *
		 * @param	name	Unique name for the sample. 
		 * 					
		 * @note	Unique name is primarily needed to more easily identify mismatched
		 * 			begin/end sample pairs. Otherwise the name in beginSamplePrecise would be enough.
		 */
		void endSamplePrecise(const String& name);

		/**
		 * @brief	Clears all sampling data, and ends any unfinished sampling blocks.
		 */
		void reset();

		/**
		 * @brief	Generates a report from all previously sampled data.
		 * 			
		 * @note	Generating a report will stop all in-progress sampling. You should make sure
		 * 			you call endSample* manually beforehand so this doesn't have to happen.
		 */
		CPUProfilerReport generateReport();

	private:
		double mBasicTimerOverhead;
		UINT64 mPreciseTimerOverhead;

		double mBasicSamplingOverheadMs;
		double mPreciseSamplingOverheadMs;
		UINT64 mBasicSamplingOverheadCycles;
		UINT64 mPreciseSamplingOverheadCycles;

		Vector<ThreadInfo*>::type mActiveThreads;
		CM_MUTEX(mThreadSync);

		void estimateTimerOverhead();
	};

	struct CM_UTILITY_EXPORT CPUProfilerBasicSamplingEntry
	{
		struct CM_UTILITY_EXPORT Data
		{
			Data();

			String name;
			UINT32 numCalls;

			double avgTimeMs;
			double maxTimeMs;
			double totalTimeMs;

			double avgSelfTimeMs;
			double totalSelfTimeMs;

			double estimatedSelfOverheadMs;
			double estimatedOverheadMs;

			float pctOfParent;
		} data;

		Vector<CPUProfilerBasicSamplingEntry>::type childEntries;
	};

	struct CM_UTILITY_EXPORT CPUProfilerPreciseSamplingEntry
	{
		struct CM_UTILITY_EXPORT Data
		{
			Data();

			String name;
			UINT32 numCalls;

			UINT64 avgCycles;
			UINT64 maxCycles;
			UINT64 totalCycles;

			UINT64 avgSelfCycles;
			UINT64 totalSelfCycles;

			UINT64 estimatedSelfOverhead;
			UINT64 estimatedOverhead;

			float pctOfParent;
		} data;

		Vector<CPUProfilerPreciseSamplingEntry>::type childEntries;
	};

	class CM_UTILITY_EXPORT CPUProfilerReport
	{
	public:
		CPUProfilerReport();

		const CPUProfilerBasicSamplingEntry& getBasicSamplingData() const { return mBasicSamplingRootEntry; }
		const CPUProfilerPreciseSamplingEntry& getPreciseSamplingData() const { return mPreciseSamplingRootEntry; }

	private:
		friend class CPUProfiler;

		CPUProfilerBasicSamplingEntry mBasicSamplingRootEntry;
		CPUProfilerPreciseSamplingEntry mPreciseSamplingRootEntry;
	};
}