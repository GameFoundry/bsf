#pragma once

#include "BsCorePrerequisites.h"
#include "BsMaterialProxy.h"
#include "BsBounds.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT RenderableSubProxy
	{
	public:
		RenderableSubProxy();

		void markBoundsDirty() { mBoundsDirty = true; }
		void markBoundsClean() { mBoundsDirty = false; }
		bool getBoundsDirty() const { return mBoundsDirty; }
		Bounds calculateWorldBounds();

		UINT32 id;
		Matrix4 worldTransform;

		MeshRenderData* mesh;
		MaterialProxy material;

		UINT64 layer;

	private:
		bool mBoundsDirty;
	};

	class BS_CORE_EXPORT RenderableProxy
	{
	public:
		Vector<RenderableSubProxy*> subProxies;
	};
}