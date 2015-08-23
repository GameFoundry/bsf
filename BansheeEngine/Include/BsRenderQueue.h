#pragma once

#include "BsPrerequisites.h"
#include "BsVector3.h"
#include "BsSubMesh.h"

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
		SPtr<MaterialCore> material;
		SPtr<MeshCoreBase> mesh;
		SubMesh subMesh;
		UINT32 passIdx;
	};

	/**
	 * @brief	Render objects determines rendering order of objects contained within it. Rendering order 
	 *			is determined by object material, and can influence rendering of transparent or opaque objects,
	 * 			or be used to improve performance by grouping similar objects together.
	 */
	class BS_EXPORT RenderQueue
	{
		/**
		 * @brief	Data used for renderable elemnt sorting.
		 */
		struct SortData
		{
			RenderQueueElement element;
			QueueSortType sortType;
			UINT32 seqIdx;
			INT32 priority;
			float distFromCamera;
		};

	public:
		RenderQueue();
		virtual ~RenderQueue() { }

		/**
		 * @brief	Adds a new entry to the render queue.
		 *
		 * @param	element			Renderable element to add to the queue.
		 * @param	distFromCamera	Distance of this object from the camera. Used for distance sorting.
		 */
		void add(RenderableElement* element, float distFromCamera);

		/**
		 * @brief	Adds a new entry to the render queue.
		 *
		 * @param	material		Material that will be used for rendering the object.
		 * @param	mesh			Mesh representing the geometry of the object.
		 * @param	subMesh			Portion of the mesh to draw.
		 * @param	distFromCamera	Distance of this object from the camera. Used for distance sorting.
		 */
		void add(const SPtr<MaterialCore>& material, const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh, float distFromCamera);

		/**
		 * @brief	Adds new entries from the provided render queue to this queue.
		 */
		void add(const RenderQueue& renderQueue);

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
		/**
		 * @brief	Callback used for sorting elements.
		 */
		static bool elementSorter(const SortData&, const SortData&);

		Set<SortData, std::function<bool(const SortData&, const SortData&)>> mRenderElements;
		Vector<RenderQueueElement> mSortedRenderElements;
	};
}