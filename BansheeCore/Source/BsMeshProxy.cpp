#include "BsMeshProxy.h"
#include "BsRenderableProxy.h"

namespace BansheeEngine
{
	MeshProxy::MeshProxy(const std::shared_ptr<VertexData>& vertexData, 
		const IndexBufferPtr& indexBuffer, const SubMesh& subMesh)
		:mVertexData(vertexData), mIndexBuffer(indexBuffer), mSubMesh(subMesh)
	{

	}

	void MeshProxy::addRenderableProxy(RenderableSubProxy* proxy)
	{
		mRenderableProxies.push_back(proxy);
	}

	void MeshProxy::removeRenderableProxy(RenderableSubProxy* proxy)
	{
		auto iterFind = std::find(mRenderableProxies.begin(), mRenderableProxies.end(), proxy);

		if (iterFind != mRenderableProxies.end())
			mRenderableProxies.erase(iterFind);
	}

	void MeshProxy::updateBounds(const Bounds& bounds)
	{
		mBounds = bounds;

		for (auto& renderableProxy : mRenderableProxies)
		{
			renderableProxy->markBoundsDirty();
		}
	}

	void MeshProxy::updateData(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer, const SubMesh& subMesh)
	{
		mVertexData = vertexData;
		mIndexBuffer = indexBuffer;
		mSubMesh = subMesh;
	}
}