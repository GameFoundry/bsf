#pragma once

#include "BsCorePrerequisites.h"
#include "BsViewport.h"
#include "BsConvexVolume.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains Camera data used by the Renderer.
	 */
	class BS_CORE_EXPORT CameraProxy
	{
	public:
		void calcWorldFrustum();

		Viewport viewport;
		Matrix4 viewMatrix;
		Matrix4 projMatrix;
		Matrix4 worldMatrix;
		INT32 priority;
		UINT64 layer;
		bool ignoreSceneRenderables;
		ConvexVolume frustum;
		ConvexVolume worldFrustum;
		Vector3 worldPosition;

		RenderQueuePtr renderQueue;
	};
}