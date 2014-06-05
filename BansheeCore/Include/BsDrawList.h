#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	struct BS_CORE_EXPORT DrawOperation
	{
		MaterialPtr material;
		MeshBasePtr mesh;
		UINT32 submeshIdx;
		Vector3 worldPosition;
	};

	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT DrawList
	{
	public:
		void add(const MaterialPtr& material, const MeshBasePtr& mesh, UINT32 submeshIdx, const Vector3& worldPosForSort);
		void clear();

		const Vector<DrawOperation>& getDrawOperations() const;

	protected:
		Vector<DrawOperation> mDrawOperations;
	};
}