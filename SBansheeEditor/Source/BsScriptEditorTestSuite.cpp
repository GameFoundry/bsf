#include "BsScriptEditorTestSuite.h"
#include "BsScriptUnitTests.h"

namespace BansheeEngine
{
	ScriptEditorTestSuite::ScriptEditorTestSuite()
	{
		BS_ADD_TEST(ScriptEditorTestSuite::runManagedTests);
	}

	void ScriptEditorTestSuite::runManagedTests()
	{
		ScriptUnitTests::runTests();
	}
}