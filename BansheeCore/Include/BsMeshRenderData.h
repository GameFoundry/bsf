#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderableProxy.h"
#include "BsBounds.h"
#include "BsSubMesh.h"

namespace BansheeEngine
{
	/**
	* Mesh render data provides a link between a Mesh and a Renderer. It may be modified
	* by both as changes occur and as the mesh is used in the renderer. Essentially it allows
	* the two to communicate without knowing about each other.
	*/
	class BS_CORE_EXPORT MeshRenderData
	{
	public:
		/**
		 * Creates a new empty render data.
		 */
		MeshRenderData() {}

		/**
		 * Creates render render data pointing to the specified vertex and index buffer, referencing
		 * the provided sub-mesh index range.
		 */
		MeshRenderData(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer, 
			const SubMesh& subMesh, UINT32 vertexOffset, std::function<void()> usedOnGPUcallback);

		/**
		 * Registers a new renderable proxy that references this mesh.
		 */
		void addRenderableProxy(RenderableElement* proxy);

		/**
		 * Unregisters a renderable proxy that references this mesh.
		 */
		void removeRenderableProxy(RenderableElement* proxy);

		/**
		 * Update mesh local bounds with the new provided bounds.
		 */
		void updateBounds(const Bounds& bounds);

		/**
		 * Get mesh local bounds.
		 */
		Bounds getBounds() const { return mBounds; }

		/**
		 * Set new internal buffers and a sub-mesh range.
		 */
		void updateData(const std::shared_ptr<VertexData>& vertexData, const IndexBufferPtr& indexBuffer, 
			const SubMesh& subMesh, UINT32 vertexOffset);

		/**
		 * Get vertex buffers.
		 */
		std::shared_ptr<VertexData> getVertexData() const { return mVertexData; }

		/**
		 * Get index buffer.
		 */
		IndexBufferPtr getIndexBuffer() const { return mIndexBuffer; }

		/**
		 * Get sub-mesh range to render.
		 */
		SubMesh getSubMesh() const { return mSubMesh; }

		/**
		 * Get vertex offset;
		 */
		UINT32 getVertexOffset() const { return mVertexOffset; }

		/**
		 * Should be called by the renderer whenever the buffers get queued for use on the GPU.
		 */
		void notifyUsedOnGPU() const;

		/**
		 * Marks the render data as invalid, usually when the parent mesh was destroyed.
		 */
		void _markAsInvalid();

	private:
		friend class Mesh;

		std::shared_ptr<VertexData> mVertexData;
		IndexBufferPtr mIndexBuffer;
		SubMesh mSubMesh;
		UINT32 mVertexOffset;

		Vector<RenderableElement*> mRenderableProxies;
		Bounds mBounds;
		std::function<void()> mUsedOnGPUCallback;
		bool mIsMeshValid;
	};
}