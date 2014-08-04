#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Abstract interface used for outputting unit test results.
	 */
	class BS_UTILITY_EXPORT TestOutput
	{
	public:
		virtual ~TestOutput() {}

		/**
		 * @brief	Triggered when a unit test fails.
		 *
		 * @param	desc		Reason why the unit test failed.
		 * @param	function	Name of the function the test failed in.
		 * @param	file		File the unit test failed in.
		 * @param	line		Line of code the unit test failed on.
		 */
		virtual void outputFail(const String& desc, const String& function, const String& file, long line) = 0;
	};

	/**
	 * @brief	Outputs unit test results so that failures are reported as exceptions. Success is not reported.
	 */
	class BS_UTILITY_EXPORT ExceptionTestOutput : public TestOutput
	{
	public:
		/**
		 * @copydoc	TestOutput::outputFail
		 */
		void outputFail(const String& desc, const String& function, const String& file, long line) final;
	};
}