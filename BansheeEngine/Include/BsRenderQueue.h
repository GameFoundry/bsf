#pragma once

#include "BsPrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Contains data needed for performing a single rendering pass.
	 */
	struct BS_EXPORT RenderQueueElement
	{
		RenderQueueElement()
			:passIdx(0)
		{ }

		RenderableProxyPtr renderable;
		RenderableElement* renderElem;
		MaterialProxyPtr material;
		MeshProxyPtr mesh;
		Vector3 worldPosition;
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
	class BS_EXPORT RenderQueue
	{
	public:
		RenderQueue();

		void add(const RenderableProxyPtr& renderable, RenderableElement* element, const Vector3& worldPosForSort);

		void add(const MaterialProxyPtr& material, const MeshProxyPtr& mesh, const Vector3& worldPosForSort);

		/**
		 * @brief	Clears all render operations from the queue.
		 */
		void clear();
		
		/**
		 * @brief	Sorts all the render operations using user-defined rules.
		 */
		virtual void sort();

		/**
		 * @brief	Returns a list of sorted render elements. Caller must ensure
		 * 			"sort" is called before this method.
		 */
		const Vector<RenderQueueElement>& getSortedElements() const;

	protected:
		Vector<RenderQueueElement> mRenderElements;
		Vector<RenderQueueElement> mSortedRenderElements;
	};
}