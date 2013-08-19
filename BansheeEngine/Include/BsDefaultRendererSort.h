#pragma once

#include "BsPrerequisites.h"
#include "BsRendererSort.h"

namespace BansheeEngine 
{
	class BS_EXPORT DefaultRendererSort : public RendererSort
	{
	public:
		CM::Vector<SortedRenderOp>::type sort(const HCamera& camera, const CM::Vector<RenderOperation>::type& renderOperations, DepthSortOrder sortOrder, bool allowSortByPass);
	};
}