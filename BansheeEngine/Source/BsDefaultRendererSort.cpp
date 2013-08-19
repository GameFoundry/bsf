#include "BsDefaultRendererSort.h"
#include "BsRenderOperation.h"
#include "CmMaterial.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Vector<SortedRenderOp>::type DefaultRendererSort::sort(const HCamera& camera, const Vector<RenderOperation>::type& renderOperations, DepthSortOrder sortOrder, bool allowSortByPass)
	{
		Vector<SortedRenderOp>::type sortedOps;

		// Just pass-through for now
		for(auto& renderOp : renderOperations)
		{
			UINT32 numPasses = renderOp.material->getNumPasses();
			for(UINT32 i = 0; i < numPasses; i++)
			{
				sortedOps.push_back(SortedRenderOp(renderOp));

				SortedRenderOp& sortedOp = sortedOps.back();
				sortedOp.passIdx = i;
			}
		}

		// TODO - Actually do some sorting

		return sortedOps;
	}
}