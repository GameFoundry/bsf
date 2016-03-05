//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsBounds.h"
#include "BsMatrix4.h"
#include "BsSubMesh.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup Renderer-Engine
	 *  @{
	 */

	/** Contains all information needed for rendering a single sub-mesh. Closely tied with Renderer. */
	class BS_EXPORT RenderableElement
	{
	public:
		/**	Contains a hardware GPU parameter buffer and index of the parameters and the slot it binds to in a material. */
		struct BS_EXPORT BufferBindInfo
		{
			BufferBindInfo(UINT32 passIdx, UINT32 paramsIdx, UINT32 slotIdx, const SPtr<GpuParamBlockBufferCore>& buffer)
				:passIdx(passIdx), paramsIdx(paramsIdx), slotIdx(slotIdx), buffer(buffer)
			{ }

			UINT32 passIdx;
			UINT32 paramsIdx;
			UINT32 slotIdx;
			SPtr<GpuParamBlockBufferCore> buffer;
		};

		/**	Reference to the mesh to render. */
		SPtr<MeshCore> mesh;

		/**	Portion of the mesh to render. */
		SubMesh subMesh;

		/**	Material to render the mesh with. */
		SPtr<MaterialCore> material;

		/**	Custom data that may optionally be set by the RenderableHanbdler. */
		Any rendererData;

		Vector<BufferBindInfo> rendererBuffers;
	};

	/** @} */
	/** @endcond */
}