//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Testing/BsTestSuite.h"

namespace bs
{

/** Runs unit tests for systems specific to the RenderBeast plugin. */
class RenderBeastTestSuite : public TestSuite
{
public:
	RenderBeastTestSuite();

private:
	void testTextureRowAllocator();
};

}  // namespace bs
