#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Slightly more fine grained version of RenderOperation, as each pass
	 * 			is specified as an individual operation. Used for sorting within the RenderQueue.
	 */
	struct BS_CORE_EXPORT SortedRenderOp
	{
		SortedRenderOp()
			:baseOperation(nullptr), passIdx(0)
		{ }

		const RenderOperation* baseOperation;
		UINT32 passIdx;
	};

	/**
	 * @brief	Render objects determines rendering order of objects contained within it. Normally the rendering
	 * 			order is determined by object material, and can influence rendering of transparent or opaque objects,
	 * 			or be used to improve performance by grouping similar objects together.
	 * 			
	 *			You need to provide your own implementation of the render queue sorting method. Likely the sorting method
	 *			will need to be closely tied to the renderer used.
	 */
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT RenderQueue
	{
	public:
		RenderQueue();

		void add(const MaterialProxy& material, MeshProxy* mesh, const Vector3& worldPosForSort);

		/**
		 * @brief	Clears all render operations from the queue.
		 */
		void clear();
		
		/**
		 * @brief	Sorts all the render operations using user-defined rules.
		 */
		virtual void sort() = 0;

		/**
		 * @brief	Returns a list of sorted render operations. Caller must ensure
		 * 			"sort" is called before this method.
		 */
		const Vector<SortedRenderOp>& getSortedRenderOps() const;

	protected:
		Vector<RenderOperation> mRenderOperations;
		Vector<SortedRenderOp> mSortedRenderOps;
	};
}