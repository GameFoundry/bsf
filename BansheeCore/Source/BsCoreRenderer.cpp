#include "BsCoreRenderer.h"
#include "BsCoreThread.h"
#include "BsRenderAPI.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	RendererMeshDataPtr CoreRenderer::_createMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
	{
		return bs_shared_ptr<RendererMeshData, PoolAlloc>(new (bs_alloc<RendererMeshData, PoolAlloc>()) 
			RendererMeshData(numVertices, numIndices, layout, indexType));
	}

	RendererMeshDataPtr CoreRenderer::_createMeshData(const MeshDataPtr& meshData)
	{
		return bs_shared_ptr<RendererMeshData, PoolAlloc>(new (bs_alloc<RendererMeshData, PoolAlloc>())
			RendererMeshData(meshData));
	}

	void CoreRenderer::_registerRenderCallback(const CameraHandlerCore* camera, INT32 index, const std::function<void()>& callback)
	{
		mRenderCallbacks[camera][index] = callback;
	}

	void CoreRenderer::_unregisterRenderCallback(const CameraHandlerCore* camera, INT32 index)
	{
		auto iterFind = mRenderCallbacks.find(camera);
		if (iterFind != mRenderCallbacks.end())
		{
			iterFind->second.erase(index);

			if (iterFind->second.empty())
				mRenderCallbacks.erase(iterFind);
		}
	}

	void CoreRenderer::setPass(const SPtr<MaterialCore>& material, UINT32 passIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		SPtr<PassCore> pass = material->getPass(passIdx);
		SPtr<PassParametersCore> passParams = material->getPassParameters(passIdx);

		if (pass->hasVertexProgram())
		{
			rs.bindGpuProgram(pass->getVertexProgram());
			rs.bindGpuParams(GPT_VERTEX_PROGRAM, passParams->mVertParams);
		}
		else
			rs.unbindGpuProgram(GPT_VERTEX_PROGRAM);

		if (pass->hasFragmentProgram())
		{
			rs.bindGpuProgram(pass->getFragmentProgram());
			rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, passParams->mFragParams);
		}
		else
			rs.unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		if (pass->hasGeometryProgram())
		{
			rs.bindGpuProgram(pass->getGeometryProgram());
			rs.bindGpuParams(GPT_GEOMETRY_PROGRAM, passParams->mGeomParams);
		}
		else
			rs.unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		if (pass->hasHullProgram())
		{
			rs.bindGpuProgram(pass->getHullProgram());
			rs.bindGpuParams(GPT_HULL_PROGRAM, passParams->mHullParams);
		}
		else
			rs.unbindGpuProgram(GPT_HULL_PROGRAM);

		if (pass->hasDomainProgram())
		{
			rs.bindGpuProgram(pass->getDomainProgram());
			rs.bindGpuParams(GPT_DOMAIN_PROGRAM, passParams->mDomainParams);
		}
		else
			rs.unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		if (pass->hasComputeProgram())
		{
			rs.bindGpuProgram(pass->getComputeProgram());
			rs.bindGpuParams(GPT_COMPUTE_PROGRAM, passParams->mComputeParams);
		}
		else
			rs.unbindGpuProgram(GPT_COMPUTE_PROGRAM);

		// TODO - Try to limit amount of state changes, if previous state is already the same

		// Set up non-texture related pass settings
		if (pass->getBlendState() != nullptr)
			rs.setBlendState(pass->getBlendState());
		else
			rs.setBlendState(BlendStateCore::getDefault());

		if (pass->getDepthStencilState() != nullptr)
			rs.setDepthStencilState(pass->getDepthStencilState(), pass->getStencilRefValue());
		else
			rs.setDepthStencilState(DepthStencilStateCore::getDefault(), pass->getStencilRefValue());

		if (pass->getRasterizerState() != nullptr)
			rs.setRasterizerState(pass->getRasterizerState());
		else
			rs.setRasterizerState(RasterizerStateCore::getDefault());
	}

	void CoreRenderer::draw(const SPtr<MeshCoreBase>& mesh, const SubMesh& subMesh)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();
		const MeshProperties& meshProps = mesh->getProperties();
		std::shared_ptr<VertexData> vertexData = mesh->getVertexData();

		rs.setVertexDeclaration(vertexData->vertexDeclaration);
		auto vertexBuffers = vertexData->getBuffers();

		if (vertexBuffers.size() > 0)
		{
			SPtr<VertexBufferCore> buffers[MAX_BOUND_VERTEX_BUFFERS];

			UINT32 endSlot = 0;
			UINT32 startSlot = MAX_BOUND_VERTEX_BUFFERS;
			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				if (iter->first >= MAX_BOUND_VERTEX_BUFFERS)
					BS_EXCEPT(InvalidParametersException, "Buffer index out of range");

				startSlot = std::min(iter->first, startSlot);
				endSlot = std::max(iter->first, endSlot);
			}

			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				buffers[iter->first - startSlot] = iter->second;
			}

			rs.setVertexBuffers(startSlot, buffers, endSlot - startSlot + 1);
		}

		rs.setDrawOperation(subMesh.drawOp);

		SPtr<IndexBufferCore> indexBuffer = mesh->getIndexBuffer();

		UINT32 indexCount = subMesh.indexCount;

		rs.setIndexBuffer(indexBuffer);
		rs.drawIndexed(subMesh.indexOffset + mesh->getIndexOffset(), indexCount, mesh->getVertexOffset(), vertexData->vertexCount);

		mesh->_notifyUsedOnGPU();
	}
}