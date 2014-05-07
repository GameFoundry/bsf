#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Slightly more fine grained version of RenderOperation, as each pass
	 * 			is specified as an individual operation. Used for sorting within the RenderQueue.
	 */
	struct CM_EXPORT SortedRenderOp
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
	class CM_EXPORT RenderQueue
	{
	public:
		RenderQueue();

		/**
		 * @brief	Adds a new render operation to the rendering queue. These operations will be sorted
		 * 			and rendered by the Renderer in sorted order.
		 *
		 * @param	material	   	The material to use for rendering the object.
		 * @param	mesh		   	The mesh to render.
		 * @param	submeshIdx	   	Sub-mesh index in "mesh" to render.
		 * @param	worldPosForSort	The world position used for sorting.
		 */
		void add(const MaterialPtr& material, const MeshBasePtr& mesh, UINT32 submeshIdx, const Vector3& worldPosForSort);

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