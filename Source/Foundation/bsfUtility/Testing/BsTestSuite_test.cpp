//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Testing/BsTestSuite.h"
#include "Platform/BsPlatform.h"
#include "Utility/BsTime.h"

namespace bs
{

	void Test::SetUpTestSuite() {
		CrashHandler::startUp();
		Platform::_startUp();
		MemStack::beginThread();
		Time::startUp();
	}

	void Test::TearDownTestSuite() {
		CrashHandler::shutDown();
		Platform::_shutDown();
		MemStack::endThread();
		Time::shutDown();
	}

} // namespace bs
