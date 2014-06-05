#pragma once

#include "BsCorePrerequisites.h"
#include "BsBounds.h"
#include "BsSubMesh.h"

namespace BansheeEngine
{
	/**
	* Mesh proxy keeps a copy of Mesh data that is used by the Renderer. It
	* also contains some Renderer specific data. Essentially it is a class that
	* links Mesh and Renderer.
	*/
	// TODO UNDOCUMENTED
	class BS_CORE_EXPORT MeshProxy
	{
	public:
		MeshProxy() {}
		MeshProxy(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer,
			const SubMesh& subMesh);

		void addRenderableProxy(RenderableSubProxy* proxy);
		void removeRenderableProxy(RenderableSubProxy* proxy);

		void updateBounds(const Bounds& bounds);
		Bounds getBounds() const { return mBounds; }

		void updateData(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer, const SubMesh& subMesh);

		std::shared_ptr<VertexData> getVertexData() const { return mVertexData; }
		IndexBufferPtr getIndexBuffer() const { return mIndexBuffer; }
		SubMesh getSubMesh() const { return mSubMesh; }

	private:
		std::shared_ptr<VertexData> mVertexData;
		IndexBufferPtr mIndexBuffer;
		SubMesh mSubMesh;

		Vector<RenderableSubProxy*> mRenderableProxies;
		Bounds mBounds;
	};
}