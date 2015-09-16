#include "BsRenderQueue.h"
#include "BsSubMesh.h"
#include "BsShader.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderableElement.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	RenderQueue::RenderQueue(StateReduction mode)
		:mStateReductionMode(mode)
	{

	}

	void RenderQueue::clear()
	{
		mSortableElements.clear();
		mSortableElementIdx.clear();
		mElements.clear();

		mSortedRenderElements.clear();
	}

	void RenderQueue::add(RenderableElement* element, float distFromCamera)
	{
		SPtr<MaterialCore> material = element->material;
		SPtr<ShaderCore> shader = material->getShader();

		mElements.push_back(element);
		
		UINT32 queuePriority = shader->getQueuePriority();
		QueueSortType sortType = shader->getQueueSortType();
		UINT32 shaderId = shader->getId();
		bool separablePasses = shader->getAllowSeparablePasses();

		switch (sortType)
		{
		case QueueSortType::None:
			distFromCamera = 0;
			break;
		case QueueSortType::BackToFront:
			distFromCamera = -distFromCamera;
			break;
		}

		UINT32 numPasses = material->getNumPasses();
		if (!separablePasses)
			numPasses = std::min(1U, numPasses);

		for (UINT32 i = 0; i < numPasses; i++)
		{
			UINT32 idx = (UINT32)mSortableElementIdx.size();
			mSortableElementIdx.push_back(idx);

			mSortableElements.push_back(SortableElement());
			SortableElement& sortableElem = mSortableElements.back();

			sortableElem.seqIdx = idx;
			sortableElem.priority = queuePriority;
			sortableElem.shaderId = shaderId;
			sortableElem.passIdx = i;
			sortableElem.distFromCamera = distFromCamera;
		}
	}

	void RenderQueue::sort()
	{
		std::function<bool(UINT32, UINT32, const Vector<SortableElement>&)> sortMethod;

		switch (mStateReductionMode)
		{
		case StateReduction::None:
			sortMethod = &elementSorterNoGroup;
			break;
		case StateReduction::Material:
			sortMethod = &elementSorterPreferGroup;
			break;
		case StateReduction::Distance:
			sortMethod = &elementSorterPreferSort;
			break;
		}

		if (mSortableElementIdx.size() > 1)
		{
			int a = 5;
		}

		// Sort only indices since we generate an entirely new data set anyway, it doesn't make sense to move sortable elements
		std::sort(mSortableElementIdx.begin(), mSortableElementIdx.end(), std::bind(sortMethod, _1, _2, mSortableElements));

		UINT32 prevShaderId = (UINT32)-1;
		UINT32 prevPassIdx = (UINT32)-1;
		RenderableElement* renderElem = nullptr;
		INT32 currentElementIdx = -1;
		UINT32 numPassesInCurrentElement = 0;
		bool separablePasses = true;
		for (UINT32 i = 0; i < (UINT32)mSortableElementIdx.size(); i++)
		{
			UINT32 idx = mSortableElementIdx[i];

			while (numPassesInCurrentElement == 0)
			{
				currentElementIdx++;
				renderElem = mElements[currentElementIdx];
				numPassesInCurrentElement = renderElem->material->getNumPasses();
				separablePasses = renderElem->material->getShader()->getAllowSeparablePasses();
			}

			const SortableElement& elem = mSortableElements[idx];
			if (separablePasses)
			{
				mSortedRenderElements.push_back(RenderQueueElement());

				RenderQueueElement& sortedElem = mSortedRenderElements.back();
				sortedElem.renderElem = renderElem;
				sortedElem.passIdx = elem.passIdx;

				if (prevShaderId != elem.shaderId || prevPassIdx != elem.passIdx)
				{
					sortedElem.applyPass = true;
					prevShaderId = elem.shaderId;
					prevPassIdx = elem.passIdx;
				}
				else
					sortedElem.applyPass = false;

				numPassesInCurrentElement--;
			}
			else
			{
				for (UINT32 j = 0; j < numPassesInCurrentElement; j++)
				{
					mSortedRenderElements.push_back(RenderQueueElement());

					RenderQueueElement& sortedElem = mSortedRenderElements.back();
					sortedElem.renderElem = renderElem;
					sortedElem.passIdx = j;
					sortedElem.applyPass = true;

					prevShaderId = elem.shaderId;
					prevPassIdx = j;
				}

				numPassesInCurrentElement = 0;
			}			
		}
	}

	bool RenderQueue::elementSorterNoGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
	{
		const SortableElement& a = lookup[aIdx];
		const SortableElement& b = lookup[bIdx];

		return (a.priority > b.priority) | (a.distFromCamera < b.distFromCamera) | (a.seqIdx < b.seqIdx);
	}

	bool RenderQueue::elementSorterPreferGroup(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
	{
		const SortableElement& a = lookup[aIdx];
		const SortableElement& b = lookup[bIdx];
		
		return (a.priority > b.priority) | (a.shaderId < b.shaderId) | (a.passIdx < b.passIdx) |
			(a.distFromCamera < b.distFromCamera) | (a.seqIdx < b.seqIdx);
	}

	bool RenderQueue::elementSorterPreferSort(UINT32 aIdx, UINT32 bIdx, const Vector<SortableElement>& lookup)
	{
		const SortableElement& a = lookup[aIdx];
		const SortableElement& b = lookup[bIdx];

		return (a.priority > b.priority) | (a.distFromCamera < b.distFromCamera) | (a.shaderId < b.shaderId) | 
			(a.passIdx < b.passIdx) | (a.seqIdx < b.seqIdx);
	}

	const Vector<RenderQueueElement>& RenderQueue::getSortedElements() const
	{
		return mSortedRenderElements;
	}
}