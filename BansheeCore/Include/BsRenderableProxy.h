#pragma once

#include "BsCorePrerequisites.h"
#include "BsMaterial.h"
#include "BsBounds.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT RenderableElement
	{
	public:
		RenderableElement();

		void markBoundsDirty() { mBoundsDirty = true; }
		void markBoundsClean() { mBoundsDirty = false; }
		bool getBoundsDirty() const { return mBoundsDirty; }
		Bounds calculateWorldBounds();

		UINT32 id;
		Matrix4 worldTransform;

		MeshRenderDataPtr mesh;
		Material::CoreProxyPtr material;

		UINT64 layer;

	private:
		bool mBoundsDirty;
	};

	class BS_CORE_EXPORT RenderableProxy
	{
	public:
		~RenderableProxy();

		Vector<RenderableElement*> renderableElements;
	};

	typedef std::shared_ptr<RenderableProxy> RenderableProxyPtr;
}