#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmModule.h"

namespace CamelotFramework
{
	/**
	 * @brief	Provides various performance measuring methods
	 */
	class CM_UTILITY_EXPORT Profiler : public Module<Profiler>
	{
	public:
		class CM_UTILITY_EXPORT Data
		{
			String name;
			float timeAvgMs;
			float timeMaxMs;
			float timeTotalMs;
			UINT32 hitCount;
		};

		class CM_UTILITY_EXPORT PreciseData
		{
			String name;
			UINT64 cyclesAvg;
			UINT64 cyclesMax;
			UINT64 cyclesTotal;
			UINT32 hitCount;

			// TODO - Add cache misses, branch mispredictions, retired instructions vs. optimal number of cycles
		};

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
		Data endSample(const String& name);

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
		PreciseData endSamplePrecise(const String& name);

		/**
		 * @brief	Called every frame. Internal method.
		 */
		void update();
	};
}