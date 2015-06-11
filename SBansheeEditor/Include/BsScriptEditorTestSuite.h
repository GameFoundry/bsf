#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsTestSuite.h"

namespace BansheeEngine
{
	class ScriptEditorTestSuite : public TestSuite
	{
	public:
		ScriptEditorTestSuite();

	private:
		void runManagedTests();
	};
}