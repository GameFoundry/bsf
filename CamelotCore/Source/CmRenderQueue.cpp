#include "CmRenderQueue.h"
#include "CmRenderOperation.h"

namespace CamelotFramework
{
	RenderQueue::RenderQueue()
	{

	}

	void RenderQueue::clear()
	{
		mRenderOperations.clear();
		mSortedRenderOps.clear();
	}

	void RenderQueue::add(const HMaterial& material, const RenderOpMesh& mesh, const Vector3& worldPosForSort)
	{
		// TODO - Make sure RenderOperations are cached so we dont allocate memory for them every frame
		mRenderOperations.push_back(RenderOperation());

		RenderOperation& renderOp = mRenderOperations.back();
		renderOp.material = material;
		renderOp.meshData = mesh;
		renderOp.worldPosition = worldPosForSort;
	}

	const Vector<SortedRenderOp>::type& RenderQueue::getSortedRenderOps() const
	{
		return mSortedRenderOps;
	}
}