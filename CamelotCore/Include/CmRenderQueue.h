#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework 
{
	struct CM_EXPORT SortedRenderOp
	{
		SortedRenderOp()
			:baseOperation(nullptr), passIdx(0)
		{ }

		const RenderOperation* baseOperation;
		UINT32 passIdx;
	};

	class CM_EXPORT RenderQueue
	{
	public:
		RenderQueue();

		void add(const MaterialPtr& material, const MeshBasePtr& mesh, UINT32 submeshIdx, const Vector3& worldPosForSort);
		void clear();
		
		virtual void sort() = 0;
		const Vector<SortedRenderOp>::type& getSortedRenderOps() const;

	protected:
		Vector<RenderOperation>::type mRenderOperations;
		Vector<SortedRenderOp>::type mSortedRenderOps;
	};
}