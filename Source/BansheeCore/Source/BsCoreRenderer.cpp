//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCoreRenderer.h"
#include "BsCoreThread.h"
#include "BsRenderAPI.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRendererExtension.h"

namespace bs
{
	CoreRenderer::CoreRenderer()
		:mCallbacks(&compareCallback)
	{ }

	SPtr<RendererMeshData> CoreRenderer::_createMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
	{
		return bs_shared_ptr<RendererMeshData>(new (bs_alloc<RendererMeshData>()) 
			RendererMeshData(numVertices, numIndices, layout, indexType));
	}

	SPtr<RendererMeshData> CoreRenderer::_createMeshData(const SPtr<MeshData>& meshData)
	{
		return bs_shared_ptr<RendererMeshData>(new (bs_alloc<RendererMeshData>())
			RendererMeshData(meshData));
	}

	bool CoreRenderer::compareCallback(const RendererExtension* a, const RendererExtension* b)
	{
		// Sort by alpha setting first, then by cull mode, then by index
		if (a->getLocation() == b->getLocation())
		{
			if (a->getPriority() == b->getPriority())
				return a > b; // Use address, at this point it doesn't matter, but std::set requires us to differentiate
			else
				return a->getPriority() > b->getPriority();
		}
		else
			return (UINT32)a->getLocation() < (UINT32)b->getLocation();
	};
}