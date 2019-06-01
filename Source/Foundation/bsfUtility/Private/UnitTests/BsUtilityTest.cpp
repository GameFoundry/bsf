//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Testing/BsConsoleTestOutput.h"
#include "Private/UnitTests/BsUtilityTestSuite.h"
#include "Platform/BsPlatform.h"
#include "Utility/BsTime.h"

using namespace bs;

int main()
{
	SPtr<TestSuite> tests = UtilityTestSuite::create<UtilityTestSuite>();

  // minimal modules necessary to precent infinite ERR loop.
  CrashHandler::startUp();
  Platform::_startUp();
  MemStack::beginThread();
  Time::startUp();


	ConsoleTestOutput testOutput;
	tests->run(testOutput);

	return 0;
}
