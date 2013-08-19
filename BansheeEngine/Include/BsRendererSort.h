#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine 
{
	enum class DepthSortOrder
	{
		FrontToBack,
		BackToFront,
		NoDepthSort
	};

	struct BS_EXPORT SortedRenderOp
	{
		SortedRenderOp(const RenderOperation& op)
			:baseOperation(op), passIdx(0)
		{ }

		const RenderOperation& baseOperation;
		CM::UINT32 passIdx;
	};

	class BS_EXPORT RendererSort
	{
	public:
		virtual CM::Vector<SortedRenderOp>::type sort(const HCamera& camera, const CM::Vector<RenderOperation>::type& renderOperations, DepthSortOrder sortOrder, bool allowSortByPass) = 0;
	};
}