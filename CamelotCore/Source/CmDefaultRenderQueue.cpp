#include "CmDefaultRenderQueue.h"
#include "CmRenderOperation.h"
#include "CmMaterial.h"

namespace CamelotFramework
{
	void DefaultRenderQueue::sort()
	{
		// Just pass-through for now
		for(auto& renderOp : mRenderOperations)
		{
			UINT32 numPasses = renderOp.material->getNumPasses();
			for(UINT32 i = 0; i < numPasses; i++)
			{
				mSortedRenderOps.push_back(SortedRenderOp());

				SortedRenderOp& sortedOp = mSortedRenderOps.back();
				sortedOp.baseOperation = &renderOp;
				sortedOp.passIdx = i;
			}
		}

		// TODO - Actually do some sorting. Use material options to sort (isTransparent, isOverlay(need to add this), etc.)
		// Note: When sorting make sure not to change order of unsorted elements. Some outside managers (like overlay and GUI) will provide render ops which are already sorted
	}
}