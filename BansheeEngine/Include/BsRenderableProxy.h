#pragma once

#include "BsPrerequisites.h"
#include "BsMaterialProxy.h"
#include "BsMeshProxy.h"
#include "BsBounds.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_EXPORT RenderableElement
	{
	public:
		RenderableElement();

		Bounds calculateWorldBounds();

		UINT32 id;
		Matrix4 worldTransform;

		MeshProxyPtr mesh;
		MaterialProxyPtr material;

		UINT64 layer;

		Any rendererData;

	private:
		bool mBoundsDirty;
	};

	class BS_EXPORT RenderableProxy
	{
	public:
		~RenderableProxy();

		Vector<RenderableElement*> renderableElements;
		RenderableType renderableType;
	};
}