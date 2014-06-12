#pragma once

#include "BsCorePrerequisites.h"
#include "BsViewport.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT CameraProxy
	{
	public:
		Viewport viewport;
		Matrix4 viewMatrix;
		Matrix4 projMatrix;
		INT32 priority;
		UINT64 layer;
		bool ignoreSceneRenderables;

		RenderQueuePtr renderQueue;
	};
}