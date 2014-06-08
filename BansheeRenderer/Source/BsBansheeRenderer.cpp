#include "BsBansheeRenderer.h"
#include "BsCamera.h"
#include "BsSceneObject.h"
#include "BsSceneManager.h"
#include "BsRenderable.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsCoreApplication.h"
#include "BsViewport.h"
#include "BsRenderTarget.h"
#include "BsRenderOperation.h"
#include "BsDefaultRenderQueue.h"
#include "BsOverlayManager.h"
#include "BsDrawHelper2D.h"
#include "BsDrawHelper3D.h"
#include "BsGUIManager.h"
#include "BsCoreThread.h"
#include "BsProfilerCPU.h"
#include "BsDrawList.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	BansheeRenderer::BansheeRenderer()
	{
		mRenderableRemovedConn = gBsSceneManager().onRenderableRemoved.connect(std::bind(&BansheeRenderer::renderableRemoved, this, _1));

		// Init compatibile material params
		RendererMaterialParams dx9params("BansheeD3D9RenderSystem", RenType_UnlitUntextured);

		// TODO - In a perfect world I would get element size and offsets by creating a dummy version of the param block
		// and reading the values.
		dx9params.addDataParam(RPS_WorldViewProjTfrm, GPT_VERTEX_PROGRAM, GPDT_MATRIX_4X4, sizeof(Matrix4), 0, 0, RP_AnyPass, RBS_PerObject);

		mRenderableMaterialParams.insert(dx9params);
	}

	BansheeRenderer::~BansheeRenderer()
	{
		mRenderableRemovedConn.disconnect();
	}

	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::addRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			mRenderableProxies.push_back(element);
			mWorldTransforms.push_back(element->worldTransform);
			mWorldBounds.push_back(element->calculateWorldBounds());
			element->markBoundsClean();

			element->id = (UINT32)(mRenderableProxies.size() - 1);
			element->mesh->addRenderableProxy(element);
		}
	}

	void BansheeRenderer::removeRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			assert(mRenderableProxies.size() > element->id && element->id >= 0);

			if (mRenderableProxies.size() == 0)
				mRenderableProxies.erase(mRenderableProxies.begin());
			else
			{
				std::swap(mRenderableProxies[element->id], mRenderableProxies.back());
				mRenderableProxies.erase(mRenderableProxies.end() - 1);

				mRenderableProxies[element->id]->id = element->id;
			}

			element->mesh->removeRenderableProxy(element);
		}
	}

	void BansheeRenderer::updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld)
	{
		for (auto& element : proxy->renderableElements)
		{
			element->worldTransform = localToWorld;

			mWorldTransforms[element->id] = localToWorld;
			mWorldBounds[element->id] = element->calculateWorldBounds();
			element->markBoundsClean();
		}
	}

	void BansheeRenderer::renderableRemoved(const HRenderable& renderable)
	{
		if (renderable->_getActiveProxy() != nullptr)
		{
			mDeletedProxies.push_back(renderable->_getActiveProxy());
		}
	}

	void BansheeRenderer::renderAll() 
	{
		gProfilerCPU().beginSample("renderA");

		gBsSceneManager().updateRenderableTransforms();

		// Remove proxies from deleted Renderable
		for (auto& proxy : mDeletedProxies)
		{
			if (proxy != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));
		}

		// Add or update Renderable proxies
		const Vector<HRenderable>& allRenderables = gBsSceneManager().getAllRenderables();
		for (auto& renderable : allRenderables)
		{
			if (!renderable->_isCoreDirty())
			{
				RenderableProxyPtr proxy = renderable->_getActiveProxy();

				if (proxy != nullptr)
					gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));

				proxy = renderable->_createProxy();
				renderable->_setActiveProxy(proxy);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addRenderableProxy, this, proxy));

				renderable->_markCoreClean();
				renderable->SO()->_markRenderDataUpToDate();
			}
			else if (!renderable->SO()->_isRenderDataUpToDate())
			{
				RenderableProxyPtr proxy = renderable->_getActiveProxy();
				assert(proxy != nullptr);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateRenderableProxy, this, proxy, renderable->SO()->getWorldTfrm()));

				renderable->SO()->_markRenderDataUpToDate();
			}
		}

		// Create frame data
		const Vector<HCamera>& allCameras = gBsSceneManager().getAllCameras();
		Vector<RenderQueuePtr> renderQueues;

		std::shared_ptr<FrameData> frameData = bs_shared_ptr<FrameData>();

		for (auto& camera : allCameras)
		{
			RenderTargetPtr target = camera->getViewport()->getTarget();
			auto findIter = std::find_if(begin(frameData->renderTargets), end(frameData->renderTargets), 
				[&target](const RenderTargetData& x) { return x.target == target; });

			if (findIter == frameData->renderTargets.end())
			{
				frameData->renderTargets.push_back(RenderTargetData());
				RenderTargetData& renderTargetData = frameData->renderTargets.back();

				renderTargetData.target = target;
				renderTargetData.cameras.push_back(CameraData());

				CameraData& camData = renderTargetData.cameras.back();
				camData.cameraProxy = camera->_createProxy();
				camData.renderQueue = bs_shared_ptr<DefaultRenderQueue>();

				renderQueues.push_back(camData.renderQueue);
			}
			else
			{
				findIter->cameras.push_back(CameraData());

				CameraData& camData = findIter->cameras.back();
				camData.cameraProxy = camera->_createProxy();
				camData.renderQueue = bs_shared_ptr<DefaultRenderQueue>();

				renderQueues.push_back(camData.renderQueue);
			}
		}

		// Sort everything based on priority
		auto cameraComparer = [&](const CameraData& a, const CameraData& b) { return a.cameraProxy.priority > b.cameraProxy.priority; };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b) { return a.target->getPriority() > b.target->getPriority(); };
		std::sort(begin(frameData->renderTargets), end(frameData->renderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : frameData->renderTargets)
		{
			Vector<CameraData>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		// Populate direct draw lists
		UINT32 idx = 0;
		for (auto& camera : allCameras)
		{
			DrawList drawList;

			// Get GUI render operations
			GUIManager::instance().render(camera->getViewport(), drawList);

			// Get overlay render operations
			OverlayManager::instance().render(camera->getViewport(), drawList);

			// Get debug render operations
			DrawHelper3D::instance().render(camera, drawList);
			DrawHelper2D::instance().render(camera, drawList);

			// Get any operations from hooked up callbacks
			const Viewport* viewportRawPtr = camera->getViewport().get();
			auto callbacksForViewport = mRenderCallbacks[viewportRawPtr];

			for (auto& callback : callbacksForViewport)
				callback(viewportRawPtr, drawList);

			RenderQueuePtr renderQueue = renderQueues[idx];

			const Vector<DrawOperation>& drawOps = drawList.getDrawOperations();
			for (auto& drawOp : drawOps)
			{
				// TODO - Will I need to check if materials match renderer?

				renderQueue->add(drawOp.material->_createProxy(),
					drawOp.mesh->_getRenderData(drawOp.submeshIdx), drawOp.worldPosition);
			}

			idx++;
		}

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::renderAllCore, this, frameData));
	}

	void BansheeRenderer::renderAllCore(std::shared_ptr<FrameData> frameData)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Render everything, target by target
		for (auto& renderTargetData : frameData->renderTargets)
		{
			RenderTargetPtr target = renderTargetData.target;
			Vector<CameraData>& cameraData = renderTargetData.cameras;

			RenderSystem::instance().beginFrame();

			for(auto& data : cameraData)
			{
				Viewport& viewport = data.cameraProxy.viewport;
				RenderSystem::instance().setViewport(viewport);

				UINT32 clearBuffers = 0;
				if(viewport.getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport.getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport.getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					RenderSystem::instance().clearViewport(clearBuffers, viewport.getClearColor(), viewport.getClearDepthValue(), viewport.getClearStencilValue());

				render(data.cameraProxy, data.renderQueue);
			}

			RenderSystem::instance().endFrame();
			RenderSystem::instance().swapBuffers(target);
		}
	}

	void BansheeRenderer::render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue) 
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		if (!cameraProxy.ignoreSceneRenderables)
		{
			// TODO - Do frustum culling
			for (auto& renderableProxy : mRenderableProxies)
			{
				// TODO - Apply world-view-proj matrix

				renderQueue->add(renderableProxy->material, renderableProxy->mesh, mWorldBounds[renderableProxy->id].getSphere().getCenter());
				
			}
		}

		Matrix4 projMatrixCstm = cameraProxy.projMatrix;
		Matrix4 viewMatrixCstm = cameraProxy.viewMatrix;

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderQueue->sort();
		const Vector<SortedRenderOp>& sortedROps = renderQueue->getSortedRenderOps();

		for(auto iter = sortedROps.begin(); iter != sortedROps.end(); ++iter)
		{
			const RenderOperation& renderOp = *iter->baseOperation;
			Material::CoreProxyPtr materialProxy = renderOp.material;

			setPass(materialProxy->passes[iter->passIdx]);
			draw(*renderOp.mesh);
		}
	}

	void BansheeRenderer::setPass(const Material::CoreProxy::PassData& pass)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		if (pass.vertexProg)
		{
			rs.bindGpuProgram(pass.vertexProg);
			rs.bindGpuParams(GPT_VERTEX_PROGRAM, *pass.vertexProgParams);
		}
		else
			rs.unbindGpuProgram(GPT_VERTEX_PROGRAM);

		if (pass.fragmentProg)
		{
			rs.bindGpuProgram(pass.fragmentProg);
			rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, *pass.geometryProgParams);
		}
		else
			rs.unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		if (pass.geometryProg)
		{
			rs.bindGpuProgram(pass.geometryProg);
			rs.bindGpuParams(GPT_GEOMETRY_PROGRAM, *pass.geometryProgParams);
		}
		else
			rs.unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		if (pass.hullProg)
		{
			rs.bindGpuProgram(pass.hullProg);
			rs.bindGpuParams(GPT_HULL_PROGRAM, *pass.hullProgParams);
		}
		else
			rs.unbindGpuProgram(GPT_HULL_PROGRAM);

		if (pass.domainProg)
		{
			rs.bindGpuProgram(pass.domainProg);
			rs.bindGpuParams(GPT_DOMAIN_PROGRAM, *pass.domainProgParams);
		}
		else
			rs.unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		if (pass.computeProg)
		{
			rs.bindGpuProgram(pass.computeProg);
			rs.bindGpuParams(GPT_COMPUTE_PROGRAM, *pass.computeProgParams);
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

	void BansheeRenderer::draw(const MeshRenderData& mesh)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();
		std::shared_ptr<VertexData> vertexData = mesh.getVertexData();

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

		SubMesh subMesh = mesh.getSubMesh();
		rs.setDrawOperation(subMesh.drawOp);

		IndexBufferPtr indexBuffer = mesh.getIndexBuffer();

		UINT32 indexCount = subMesh.indexCount;
		if (indexCount == 0)
			indexCount = indexBuffer->getNumIndices();

		rs.setIndexBuffer(indexBuffer);
		rs.drawIndexed(subMesh.indexOffset, indexCount, mesh.getVertexOffset(), vertexData->vertexCount);

		mesh.notifyUsedOnGPU();
	}
}