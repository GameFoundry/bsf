#include "BsRenderableProxy.h"

namespace BansheeEngine
{
	RenderableElement::RenderableElement()
		:mBoundsDirty(false), id(0), mesh(nullptr)
	{ }

	Bounds RenderableElement::calculateWorldBounds()
	{
		if (mesh == nullptr)
			return Bounds();

		Bounds worldBounds = mesh->bounds;
		worldBounds.transformAffine(worldTransform);

		return worldBounds;
	}

	RenderableProxy::~RenderableProxy()
	{
		for (auto& renderableElement : renderableElements)
			bs_delete(renderableElement);
	}
}