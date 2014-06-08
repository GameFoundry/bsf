#include "BsRenderQueue.h"
#include "BsRenderOperation.h"
#include "BsMeshRenderData.h"

namespace BansheeEngine
{
	RenderQueue::RenderQueue()
	{

	}

	void RenderQueue::clear()
	{
		mRenderOperations.clear();
		mSortedRenderOps.clear();
	}

	void RenderQueue::add(const Material::CoreProxyPtr& material, const MeshRenderDataPtr& mesh, const Vector3& worldPosForSort)
	{
		// TODO - Make sure RenderOperations are cached so we dont allocate memory for them every frame
		mRenderOperations.push_back(RenderOperation());

		RenderOperation& renderOp = mRenderOperations.back();
		renderOp.material = material;
		renderOp.mesh = mesh;
		renderOp.worldPosition = worldPosForSort;
	}

	const Vector<SortedRenderOp>& RenderQueue::getSortedRenderOps() const
	{
		return mSortedRenderOps;
	}
}