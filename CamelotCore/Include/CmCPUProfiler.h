#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	class CPUProfilerReport;

	/**
	 * @brief	Provides various performance measuring methods
	 * 			
	 * @note	This class is thread safe. Matching begin*\end* calls
	 * 			must belong to the same thread though.
	 */
	class CM_EXPORT CPUProfiler
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
			ProfileSample(double _time, UINT64 _numAllocs, UINT64 _numFrees)
				:time(_time), numAllocs(_numAllocs), numFrees(_numFrees)
			{ }

			double time;
			UINT64 numAllocs;
			UINT64 numFrees;
		};

		struct PreciseProfileSample
		{
			PreciseProfileSample(UINT64 _cycles, UINT64 _numAllocs, UINT64 _numFrees)
				:cycles(_cycles), numAllocs(_numAllocs), numFrees(_numFrees)
			{ }

			UINT64 cycles;
			UINT64 numAllocs;
			UINT64 numFrees;
		};

		struct ProfileData
		{
			ProfilerVector<ProfileSample> samples;
			Timer timer;

			UINT64 memAllocs;
			UINT64 memFrees;

			void beginSample();
			void endSample();
			void resumeLastSample();
		};

		struct PreciseProfileData
		{
			ProfilerVector<PreciseProfileSample> samples;
			TimerPrecise timer;

			UINT64 memAllocs;
			UINT64 memFrees;

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

			ProfilerString name;
			
			ProfileData basic;
			PreciseProfileData precise;

			ProfilerVector<ProfiledBlock*> children;

			ProfiledBlock* findChild(const ProfilerString& name) const;
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

			ProfilerStack<ActiveBlock> activeBlocks;
			ActiveBlock activeBlock;

			void begin(const ProfilerString& _name);
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
		void beginThread(const ProfilerString& name);

		/**
		 * @brief	Ends sampling for the current thread. No beginSample*\endSample* calls after this point.
		 */
		void endThread();

		/**
		 * @brief	Begins sample measurement. Must be followed by endSample. 
		 *
		 * @param	name	Unique name for the sample you can later use to find the sampling data.
		 */
		void beginSample(const ProfilerString& name);

		/**
		 * @brief	Ends sample measurement.
		 *
		 * @param	name	Unique name for the sample. 
		 * 					
		 * @note	Unique name is primarily needed to more easily identify mismatched
		 * 			begin/end sample pairs. Otherwise the name in beginSample would be enough.
		 */
		void endSample(const ProfilerString& name);

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
		void beginSamplePrecise(const ProfilerString& name);

		/**
		 * @brief	Ends precise sample measurement.
		 *
		 * @param	name	Unique name for the sample. 
		 * 					
		 * @note	Unique name is primarily needed to more easily identify mismatched
		 * 			begin/end sample pairs. Otherwise the name in beginSamplePrecise would be enough.
		 */
		void endSamplePrecise(const ProfilerString& name);

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

		ProfilerVector<ThreadInfo*> mActiveThreads;
		CM_MUTEX(mThreadSync);

		void estimateTimerOverhead();
	};

	struct CM_EXPORT CPUProfilerBasicSamplingEntry
	{
		struct CM_EXPORT Data
		{
			Data();

			String name;
			UINT32 numCalls;

			UINT64 memAllocs;
			UINT64 memFrees;

			double avgTimeMs;
			double maxTimeMs;
			double totalTimeMs;

			double avgSelfTimeMs;
			double totalSelfTimeMs;

			double estimatedSelfOverheadMs;
			double estimatedOverheadMs;

			float pctOfParent;
		} data;

		ProfilerVector<CPUProfilerBasicSamplingEntry> childEntries;
	};

	struct CM_EXPORT CPUProfilerPreciseSamplingEntry
	{
		struct CM_EXPORT Data
		{
			Data();

			String name;
			UINT32 numCalls;

			UINT64 memAllocs;
			UINT64 memFrees;

			UINT64 avgCycles;
			UINT64 maxCycles;
			UINT64 totalCycles;

			UINT64 avgSelfCycles;
			UINT64 totalSelfCycles;

			UINT64 estimatedSelfOverhead;
			UINT64 estimatedOverhead;

			float pctOfParent;
		} data;

		ProfilerVector<CPUProfilerPreciseSamplingEntry> childEntries;
	};

	class CM_EXPORT CPUProfilerReport
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