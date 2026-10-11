//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Testing/B3DTestSuite.h"

namespace b3d
{
	/** Tests render::FrameGraph: culling, transient placement, barrier planning and the development checks. */
	class FrameGraphTestSuite : public TestSuite
	{
	public:
		FrameGraphTestSuite();

	private:
#if B3D_GPU_EXPLICIT_BARRIERS
		/**
		 * A producer, two unrelated passes and a consumer: the consumer's barrier is released after the producer and acquired before the
		 * consumer, and the consumer reads what the producer wrote.
		 */
		void TestSplitBarriers();

		/**
		 * Two clears of a transient followed by a read: the first clear is culled, as is a pass whose writes nothing reads. Raster passes
		 * render into attachments through a render texture the graph builds.
		 */
		void TestCulling();

		/**
		 * A transient whose memory earlier transients used gets an alias acquire, released after the last pass of the earlier transients
		 * when passes lie between that pass and its first use.
		 */
		void TestSplitAliasAcquire();

		/** An extracted transient survives the graph at rest in its own memory, and a later graph imports and reads it. */
		void TestExtract();

		/** An exported resource and a resource returned at the end of the graph are both at rest for the next graph that imports them. */
		void TestExport();

#if B3D_BUILD_TYPE_DEVELOPMENT
		/**
		 * The development checks: writes to resources at rest, conflicting declarations in one pass, writes after an export, dependencies
		 * between dispatches of one pass, and writes through parameter sets the pass did not create.
		 */
		void TestValidation();
#endif
#endif
	};
} // namespace b3d
