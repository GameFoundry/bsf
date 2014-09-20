#include "BsRenderer.h"
#include "BsCoreThread.h"
#include "BsRenderSystem.h"
#include "BsMaterialProxy.h"
#include "BsMeshProxy.h"
#include "BsMesh.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	void Renderer::setPass(const MaterialProxy& material, UINT32 passIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		const MaterialProxyPass& pass = material.passes[passIdx];
		if (pass.vertexProg)
		{
			rs.bindGpuProgram(pass.vertexProg);
			rs.bindGpuParams(GPT_VERTEX_PROGRAM, material.params[pass.vertexProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_VERTEX_PROGRAM);

		if (pass.fragmentProg)
		{
			rs.bindGpuProgram(pass.fragmentProg);
			rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, material.params[pass.fragmentProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		if (pass.geometryProg)
		{
			rs.bindGpuProgram(pass.geometryProg);
			rs.bindGpuParams(GPT_GEOMETRY_PROGRAM, material.params[pass.geometryProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		if (pass.hullProg)
		{
			rs.bindGpuProgram(pass.hullProg);
			rs.bindGpuParams(GPT_HULL_PROGRAM, material.params[pass.hullProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_HULL_PROGRAM);

		if (pass.domainProg)
		{
			rs.bindGpuProgram(pass.domainProg);
			rs.bindGpuParams(GPT_DOMAIN_PROGRAM, material.params[pass.domainProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		if (pass.computeProg)
		{
			rs.bindGpuProgram(pass.computeProg);
			rs.bindGpuParams(GPT_COMPUTE_PROGRAM, material.params[pass.computeProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_COMPUTE_PROGRAM);

		// TODO - Try to limit amount of state changes, if previous state is already the same

		// Set up non-texture related pass settings
		if (pass.blendState != nullptr)
			rs.setBlendState(pass.blendState.getInternalPtr());
		else
			rs.setBlendState(BlendState::getDefault());

		if (pass.depthStencilState != nullptr)
			rs.setDepthStencilState(pass.depthStencilState.getInternalPtr(), pass.stencilRefValue);
		else
			rs.setDepthStencilState(DepthStencilState::getDefault(), pass.stencilRefValue);

		if (pass.rasterizerState != nullptr)
			rs.setRasterizerState(pass.rasterizerState.getInternalPtr());
		else
			rs.setRasterizerState(RasterizerState::getDefault());
	}

	void Renderer::draw(const MeshProxy& meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();
		MeshBasePtr mesh;

		// TODO: Instead of this lock consider just storing all needed data in MeshProxy and not referencing Mesh at all?
		if (!meshProxy.mesh.expired())
			mesh = meshProxy.mesh.lock();
		else
			return;

		std::shared_ptr<VertexData> vertexData = mesh->_getVertexData();

		rs.setVertexDeclaration(vertexData->vertexDeclaration);
		auto vertexBuffers = vertexData->getBuffers();

		if (vertexBuffers.size() > 0)
		{
			VertexBufferPtr buffers[MAX_BOUND_VERTEX_BUFFERS];

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

		SubMesh subMesh = meshProxy.subMesh;
		rs.setDrawOperation(subMesh.drawOp);

		IndexBufferPtr indexBuffer = mesh->_getIndexBuffer();

		UINT32 indexCount = subMesh.indexCount;
		if (indexCount == 0)
			indexCount = indexBuffer->getNumIndices();

		rs.setIndexBuffer(indexBuffer);
		rs.drawIndexed(subMesh.indexOffset + mesh->_getIndexOffset(), indexCount, mesh->_getVertexOffset(), vertexData->vertexCount);

		mesh->_notifyUsedOnGPU();
	}
}