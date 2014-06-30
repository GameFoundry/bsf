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

		RenderableElement* renderElem;
		MaterialProxyPtr material;
		MeshProxyPtr mesh;
		Vector3 worldPosition;
		UINT32 passIdx;
	};

	/**
	 * @brief	Render objects determines rendering order of objects contained within it. Rendering order 
	 *			is determined by object material, and can influence rendering of transparent or opaque objects,
	 * 			or be used to improve performance by grouping similar objects together.
	 */
	class BS_EXPORT RenderQueue
	{
	public:
		RenderQueue();

		/**
		 * @brief	Adds a new entry to the render queue.
		 *
		 * @param	element			Renderable element to add to the queue.
		 * @param	worldPosForSort	World position that will be used for distance sorting of the object.
		 */
		void add(RenderableElement* element, const Vector3& worldPosForSort);

		/**
		 * @brief	Adds a new entry to the render queue.
		 *
		 * @param	material	Material that will be used for rendering the object.
		 * @param	mesh		Mesh representing the geometry of the object.
		 * @param	worldPosForSort	World position that will be used for distance sorting of the object.
		 */
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
		 * @brief	Returns internal list of render elements (unsorted).
		 */
		const Vector<RenderQueueElement>& getElements() const { return mRenderElements; }

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