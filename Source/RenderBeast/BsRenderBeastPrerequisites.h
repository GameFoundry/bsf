//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"

/** @addtogroup Plugins
 *  @{
 */

/** @defgroup RenderBeast RenderBeast
 *	Banshee's default renderer implementation.
 */

/** @} */

namespace bs { namespace ct
{
	/**
	 * Determines the feature set to be used by RenderBeast. Feature sets control the quality and type of rendering
	 * effects depending on available hardware (For example a desktop computer can handle higher end rendering than a
	 * mobile device).
	 */
	enum class RenderBeastFeatureSet
	{
		/** High end feature set utilizing the latest and greatest effects. */
		Desktop,
		/** Mid-range feature set optimized for macOS and its obsolete OpenGL 4.1 version. */
		DesktopMacOS
	};

	class ObjectRenderer;
	struct RenderBeastOptions;
	struct PooledRenderTexture;
	class RenderTargets;
	class RendererView;
	struct LightData;
}}