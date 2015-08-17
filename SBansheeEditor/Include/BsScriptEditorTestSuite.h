#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsTestSuite.h"

namespace BansheeEngine
{
	/**
	 * @brief	Performs editor managed unit tests.
	 */
	class ScriptEditorTestSuite : public TestSuite
	{
	public:
		ScriptEditorTestSuite();

	private:
		/**
		 * @brief	Triggers execution of managed unit tests.
		 */
		void runManagedTests();
	};
}