//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a single drawable object
	 *			stored in a DrawList.
	 */
	struct BS_CORE_EXPORT DrawOperation
	{
		MaterialPtr material;
		MeshBasePtr mesh;
		UINT32 submeshIdx;
		Vector3 worldPosition;
	};

	/**
	 * @brief	A container you may use to queue
	 *			custom drawable objects in.
	 */
	class BS_CORE_EXPORT DrawList
	{
	public:
		/**
		 * @brief	Adds a new drawable object.
		 *
		 * @param	material		Material to draw the object with.
		 * @param	mesh			Parent mesh of the object.
		 * @param	submeshIdx		Index of the sub-mesh to render, in the parent mesh.
		 * @param	worldPosForSort	World position of the object that will be used for distance
		 *							sorting before rendering.
		 */
		void add(const MaterialPtr& material, const MeshBasePtr& mesh, UINT32 submeshIdx, const Vector3& worldPosForSort);

		/**
		 * @brief	Removes all queued drawable objects.
		 */
		void clear();

		/**
		 * @brief	Returns all queued drawable objects.
		 */
		const Vector<DrawOperation>& getDrawOperations() const;

	protected:
		Vector<DrawOperation> mDrawOperations;
	};
}