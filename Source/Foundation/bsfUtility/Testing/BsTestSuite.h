//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <gtest/gtest.h>
#include "Prerequisites/BsPrerequisitesUtil.h"



namespace bs
{

class Test : public ::testing::Test {
protected:
	// setup once for all tests within the test suite.
  static void SetUpTestSuite();
  static void TearDownTestSuite();
};

} // namespace bs
