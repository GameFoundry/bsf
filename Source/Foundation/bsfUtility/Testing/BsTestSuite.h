//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include <gtest/gtest.h>
#include "Prerequisites/BsPrerequisitesUtil.h"
#include "BsCorePrerequisites.h"

namespace bs
{

/*
* AppEnvironment is a subclass of googletest Environment. It is setup and
* torn-down once globally. It is useful to us for to setup the application.
* Unit-tests can all be run under the same application for the most part, so
* long as scenes are cleaned up in each test suite.
*/
class AppEnvironment : public ::testing::Environment {
	public:

		// Override this to define how to set up the environment.
		void SetUp() override;

		// Override this to define how to tear down the environment.
		void TearDown() override;
};


class Test : public ::testing::Test {
	public:
		// placeholder
		static void TearDownTestSuite() {}
		// placeholder
		static void SetUpTestSuite() {}
	protected:
		// utility function for navigating 3d unit-test scenes.
		HSceneObject addFlyableCamera();
};

}  // namespace bs