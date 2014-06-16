#include "BsRenderQueue.h"
#include "BsMaterialProxy.h"
#include "BsMeshProxy.h"
#include "BsRenderableProxy.h"

namespace BansheeEngine
{
	RenderQueue::RenderQueue()
	{

	}

	void RenderQueue::clear()
	{
		mRenderElements.clear();
		mSortedRenderElements.clear();
	}

	void RenderQueue::add(const RenderableProxyPtr& renderable, RenderableElement* element, const Vector3& worldPosForSort)
	{
		// TODO - Make sure RenderQueueElements are cached so we dont allocate memory for them every frame
		mRenderElements.push_back(RenderQueueElement());

		RenderQueueElement& renderOp = mRenderElements.back();
		renderOp.renderable = renderable;
		renderOp.renderElem = element;
		renderOp.material = element->material;
		renderOp.mesh = element->mesh;
		renderOp.worldPosition = worldPosForSort;
	}

	void RenderQueue::add(const MaterialProxyPtr& material, const MeshProxyPtr& mesh, const Vector3& worldPosForSort)
	{
		// TODO - Make sure RenderQueueElements are cached so we dont allocate memory for them every frame
		mRenderElements.push_back(RenderQueueElement());

		RenderQueueElement& renderOp = mRenderElements.back();
		renderOp.renderable = nullptr;
		renderOp.renderElem = nullptr;
		renderOp.material = material;
		renderOp.mesh = mesh;
		renderOp.worldPosition = worldPosForSort;
	}

	void RenderQueue::sort()
	{
		// Just pass-through for now
		for (auto& renderElem : mRenderElements)
		{
			UINT32 numPasses = (UINT32)renderElem.renderElem->material->passes.size();
			for (UINT32 i = 0; i < numPasses; i++)
			{
				mSortedRenderElements.push_back(RenderQueueElement());

				RenderQueueElement& sortedElem = mSortedRenderElements.back();
				sortedElem.renderable = renderElem.renderable;
				sortedElem.renderElem = renderElem.renderElem;
				sortedElem.material = renderElem.material;
				sortedElem.mesh = renderElem.mesh;
				sortedElem.worldPosition = renderElem.worldPosition;
				sortedElem.passIdx = i;
			}
		}

		// TODO - Actually do some sorting. Use material options to sort (isTransparent, isOverlay(need to add this), etc.)
		// Note: When sorting make sure not to change order of unsorted elements. Some outside managers (like overlay and GUI) will provide render ops which are already sorted
	}

	const Vector<RenderQueueElement>& RenderQueue::getSortedElements() const
	{
		return mSortedRenderElements;
	}
}