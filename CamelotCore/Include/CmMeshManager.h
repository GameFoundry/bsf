#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotFramework
{
	class CM_EXPORT MeshManager : public Module<MeshManager>
	{
	public:
		MeshManager();
		~MeshManager();

		MeshPtr create();
		MeshPtr createEmpty();

		/**
		 * @brief	Returns empty mesh data. (Technically it is not empty,
		 * 			as 0 sized buffers will cause problems, so it contains 3 indices
		 * 			and 1 vertex).
		 */
		MeshDataPtr getNullMeshData() const { return mNullMeshData; }

	private:
		MeshDataPtr mNullMeshData;
	};
}