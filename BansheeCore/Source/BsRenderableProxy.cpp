#include "BsRenderableProxy.h"
#include "BsMeshRenderData.h"

namespace BansheeEngine
{
	RenderableSubProxy::RenderableSubProxy()
		:mBoundsDirty(false), id(0), mesh(nullptr)
	{ }

	Bounds RenderableSubProxy::calculateWorldBounds()
	{
		if (mesh == nullptr)
			return Bounds();

		Bounds worldBounds = mesh->getBounds();
		worldBounds.transformAffine(worldTransform);

		return worldBounds;
	}
}