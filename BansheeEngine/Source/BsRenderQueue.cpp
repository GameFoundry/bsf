#include "BsRenderQueue.h"
#include "BsMaterialProxy.h"
#include "BsRenderableProxy.h"
#include "BsSubMesh.h"
#include "BsShader.h"
#include "BsMesh.h"

namespace BansheeEngine
{
	RenderQueue::RenderQueue()
		:mRenderElements(&elementSorter)
	{

	}

	void RenderQueue::clear()
	{
		mRenderElements.clear();
		mSortedRenderElements.clear();
	}

	void RenderQueue::add(RenderableElement* element, float distFromCamera)
	{
		SortData sortData;

		RenderQueueElement& renderOp = sortData.element;
		renderOp.renderElem = element;
		renderOp.material = element->material;
		renderOp.mesh = element->mesh;
		renderOp.subMesh = element->subMesh;

		sortData.distFromCamera = distFromCamera;
		sortData.priority = element->material->shader->getQueuePriority();
		sortData.sortType = element->material->shader->getQueueSortType();
		sortData.seqIdx = (UINT32)mRenderElements.size();

		// TODO - Make sure elements are cached so we dont allocate memory for them every frame
		mRenderElements.insert(sortData);
	}

	void RenderQueue::add(const MaterialProxyPtr& material, const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh, float distFromCamera)
	{
		SortData sortData;

		RenderQueueElement& renderOp = sortData.element;
		renderOp.renderElem = nullptr;
		renderOp.material = material;
		renderOp.mesh = mesh;
		renderOp.subMesh = subMesh;

		sortData.distFromCamera = distFromCamera;
		sortData.priority = material->shader->getQueuePriority();
		sortData.sortType = material->shader->getQueueSortType();
		sortData.seqIdx = (UINT32)mRenderElements.size();

		// TODO - Make sure elements are cached so we dont allocate memory for them every frame
		mRenderElements.insert(sortData);
	}

	void RenderQueue::add(const RenderQueue& renderQueue)
	{
		for (auto& elem : renderQueue.mRenderElements)
		{
			if (elem.element.renderElem != nullptr)
				add(elem.element.renderElem, elem.distFromCamera);
			else
				add(elem.element.material, elem.element.mesh, elem.element.subMesh, elem.distFromCamera);
		}
	}

	void RenderQueue::sort()
	{
		// TODO - I'm ignoring "separate pass" material parameter.
		for (auto& sortData : mRenderElements)
		{
			const RenderQueueElement& renderElem = sortData.element;
			UINT32 numPasses = (UINT32)renderElem.material->passes.size();
			for (UINT32 i = 0; i < numPasses; i++)
			{
				mSortedRenderElements.push_back(RenderQueueElement());

				RenderQueueElement& sortedElem = mSortedRenderElements.back();
				sortedElem.renderElem = renderElem.renderElem;
				sortedElem.material = renderElem.material;
				sortedElem.mesh = renderElem.mesh;
				sortedElem.subMesh = renderElem.subMesh;
				sortedElem.passIdx = i;
			}
		}
	}

	bool RenderQueue::elementSorter(const SortData& a, const SortData& b)
	{
		if (a.priority == b.priority)
		{
			if (a.sortType == QueueSortType::None || a.sortType != b.sortType)
				return a.seqIdx < b.seqIdx;

			if (a.distFromCamera == b.distFromCamera)
				return a.seqIdx < b.seqIdx;

			if (a.sortType == QueueSortType::FrontToBack)
				return a.distFromCamera < b.distFromCamera;
			else
				return a.distFromCamera > b.distFromCamera;
		}

		return a.priority > b.priority;
	}

	const Vector<RenderQueueElement>& RenderQueue::getSortedElements() const
	{
		return mSortedRenderElements;
	}
}