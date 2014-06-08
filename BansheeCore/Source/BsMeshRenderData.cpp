#include "BsMeshRenderData.h"
#include "BsRenderableProxy.h"

namespace BansheeEngine
{
	MeshRenderData::MeshRenderData(const std::shared_ptr<VertexData>& vertexData, 
		const IndexBufferPtr& indexBuffer, const SubMesh& subMesh, UINT32 vertexOffset, std::function<void()> usedOnGPUcallback)
		:mVertexData(vertexData), mIndexBuffer(indexBuffer), mSubMesh(subMesh), 
		mUsedOnGPUCallback(usedOnGPUcallback), mVertexOffset(vertexOffset), mIsMeshValid(true)
	{

	}

	void MeshRenderData::addRenderableProxy(RenderableElement* proxy)
	{
		mRenderableProxies.push_back(proxy);
	}

	void MeshRenderData::removeRenderableProxy(RenderableElement* proxy)
	{
		auto iterFind = std::find(mRenderableProxies.begin(), mRenderableProxies.end(), proxy);

		if (iterFind != mRenderableProxies.end())
			mRenderableProxies.erase(iterFind);
	}

	void MeshRenderData::updateBounds(const Bounds& bounds)
	{
		mBounds = bounds;

		for (auto& renderableProxy : mRenderableProxies)
		{
			renderableProxy->markBoundsDirty();
		}
	}

	void MeshRenderData::updateData(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer, 
		const SubMesh& subMesh, UINT32 vertexOffset)
	{
		mVertexData = vertexData;
		mIndexBuffer = indexBuffer;
		mSubMesh = subMesh;
	}

	void MeshRenderData::notifyUsedOnGPU() const
	{
		if (mIsMeshValid && mUsedOnGPUCallback != nullptr)
			mUsedOnGPUCallback();
	}

	void MeshRenderData::_markAsInvalid()
	{
		mIsMeshValid = false;
	}
}