//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDrawList.h"

namespace BansheeEngine
{
	void DrawList::clear()
	{
		mDrawOperations.clear();
	}

	void DrawList::add(const MaterialPtr& material, const MeshBasePtr& mesh, UINT32 submeshIdx, const Vector3& worldPosForSort)
	{
		// TODO - Make sure RenderOperations are cached so we dont allocate memory for them every frame
		mDrawOperations.push_back(DrawOperation());

		DrawOperation& renderOp = mDrawOperations.back();
		renderOp.material = material;
		renderOp.mesh = mesh;
		renderOp.worldPosition = worldPosForSort;
		renderOp.submeshIdx = submeshIdx;
	}

	const Vector<DrawOperation>& DrawList::getDrawOperations() const
	{
		return mDrawOperations;
	}
}