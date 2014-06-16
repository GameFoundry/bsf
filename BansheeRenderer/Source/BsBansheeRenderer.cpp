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
#include "BsRenderQueue.h"
#include "BsOverlayManager.h"
#include "BsDrawHelper2D.h"
#include "BsDrawHelper3D.h"
#include "BsGUIManager.h"
#include "BsCoreThread.h"
#include "BsGpuParams.h"
#include "BsProfilerCPU.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsDrawList.h"
#include "BsHardwareBufferManager.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsShader.h"
#include "BsShaderProxy.h"
#include "BsTime.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	BansheeRenderer::BansheeRenderer()
	{
		mRenderableRemovedConn = gBsSceneManager().onRenderableRemoved.connect(std::bind(&BansheeRenderer::renderableRemoved, this, _1));
		mCameraRemovedConn = gBsSceneManager().onCameraRemoved.connect(std::bind(&BansheeRenderer::cameraRemoved, this, _1));

		// Init default shaders
		initRenderableLitTexturedData();
	}

	BansheeRenderer::~BansheeRenderer()
	{
		mRenderableRemovedConn.disconnect();
		mCameraRemovedConn.disconnect();
	}

	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::initRenderableLitTexturedData()
	{
		mLitTexturedData.defaultShader = getDefaultShader(RenType_LitTextured);
		TechniquePtr defaultTechnique = mLitTexturedData.defaultShader->getBestTechnique();
		PassPtr defaultPass = defaultTechnique->getPass(0);

		bool matrixTranspose = defaultPass->getVertexProgram()->requiresMatrixTranspose(); // This is a static setting across all GPU programs
		const GpuParamDesc& vertParamDesc = defaultPass->getVertexProgram()->getParamDesc();
		const GpuParamDesc& fragParamDesc = defaultPass->getFragmentProgram()->getParamDesc();

		GpuParamDesc staticParamsDesc;
		GpuParamDesc perFrameParamsDesc;
		GpuParamDesc perObjectParamsDesc;

		const Map<String, SHADER_DATA_PARAM_DESC>& dataParams = mLitTexturedData.defaultShader->_getDataParams();
		for (auto& param : dataParams)
		{
			if (param.second.rendererSemantic == RPS_LightDir)
			{
				auto iterFind = fragParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == fragParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.lightDirParamDesc = iterFind->second;
				staticParamsDesc.params[iterFind->first] = iterFind->second;
			}
			else if (param.second.rendererSemantic == RPS_Time)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.timeParamDesc = iterFind->second;
				perFrameParamsDesc.params[iterFind->first] = iterFind->second;
			}
			else if (param.second.rendererSemantic == RPS_WorldViewProjTfrm)
			{
				auto iterFind = vertParamDesc.params.find(param.second.gpuVariableName);
				if (iterFind == vertParamDesc.params.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.wvpParamDesc = iterFind->second;
				perObjectParamsDesc.params[iterFind->first] = iterFind->second;
			}
		}

		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlocks = mLitTexturedData.defaultShader->_getParamBlocks();
		for (auto& block : paramBlocks)
		{
			if (block.second.rendererSemantic == RBS_Static)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.staticParamBlockDesc = iterFind->second;
				staticParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
			else if (block.second.rendererSemantic == RBS_PerFrame)
			{
				auto iterFind = fragParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == fragParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.perFrameParamBlockDesc = iterFind->second;
				perFrameParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
			else if (block.second.rendererSemantic == RBS_PerObject)
			{
				auto iterFind = vertParamDesc.paramBlocks.find(block.second.name);
				if (iterFind == vertParamDesc.paramBlocks.end())
					BS_EXCEPT(InternalErrorException, "Invalid default shader.");

				mLitTexturedData.perObjectParamBlockDesc = iterFind->second;
				perObjectParamsDesc.paramBlocks[iterFind->first] = iterFind->second;
			}
		}

		// Create global GPU param buffers and get parameter handles
		mLitTexturedData.staticParams = bs_shared_ptr<GpuParams>(staticParamsDesc, matrixTranspose);
		mLitTexturedData.perFrameParams = bs_shared_ptr<GpuParams>(perFrameParamsDesc, matrixTranspose);

		mLitTexturedData.staticParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(mLitTexturedData.staticParamBlockDesc.blockSize);
		mLitTexturedData.perFrameParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(mLitTexturedData.perFrameParamBlockDesc.blockSize);

		mLitTexturedData.staticParams->setParamBlockBuffer(mLitTexturedData.staticParamBlockDesc.slot, mLitTexturedData.staticParamBuffer);
		mLitTexturedData.perFrameParams->setParamBlockBuffer(mLitTexturedData.perFrameParamBlockDesc.slot, mLitTexturedData.perFrameParamBuffer);

		mLitTexturedData.staticParams->getParam(mLitTexturedData.lightDirParamDesc.name, mLitTexturedData.lightDirParam);
		mLitTexturedData.staticParams->getParam(mLitTexturedData.timeParamDesc.name, mLitTexturedData.timeParam);

		mLitTexturedData.lightDirParam.set(Vector4(0.707f, 0.707f, 0.707f, 0.0f));
	}

	ShaderPtr BansheeRenderer::getDefaultShader(RenderableType type) const
	{
		return nullptr;
	}

	void BansheeRenderer::addRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			mRenderableElements.push_back(element);
			mWorldTransforms.push_back(element->worldTransform);
			mWorldBounds.push_back(element->calculateWorldBounds());

			element->id = (UINT32)(mRenderableElements.size() - 1);
		}

		// Find and set up renderer buffers and parameters
		static auto paramsMatch = [](const GpuParamDataDesc& a, const GpuParamDataDesc& b)
		{
			return a.gpuMemOffset == b.gpuMemOffset && a.elementSize == b.elementSize &&
				a.arraySize == b.arraySize && a.arrayElementStride == b.arrayElementStride;
		};

		if (proxy->renderableType == RenType_LitTextured)
		{
			for (auto& element : proxy->renderableElements)
			{
				element->rendererData = RenderLitTexturedPerObjectData();
				RenderLitTexturedPerObjectData* rendererData = any_cast_unsafe<RenderLitTexturedPerObjectData>(&element->rendererData);

				ShaderProxyPtr shader = element->material->shader;

				const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlockDescs = shader->paramBlocks;
				const Map<String, SHADER_DATA_PARAM_DESC>& dataParamDescs = shader->dataParams;
				String staticBlockName;
				String perFrameBlockName;
				String perObjectBlockName;

				String wvpParamName;

				for (auto& paramBlockDesc : paramBlockDescs)
				{
					switch (paramBlockDesc.second.rendererSemantic)
					{
					case RBS_Static:
						staticBlockName = paramBlockDesc.second.name;
						break;
					case RBS_PerFrame:
						perFrameBlockName = paramBlockDesc.second.name;
						break;
					case RBS_PerObject:
						perObjectBlockName = paramBlockDesc.second.name;
						break;
					}
				}

				for (auto& paramDesc : dataParamDescs)
				{
					if (paramDesc.second.rendererSemantic == RPS_WorldViewProjTfrm)
						wvpParamName = paramDesc.second.gpuVariableName;
				}

				UINT32 idx = 0;
				for (auto& gpuParams : element->material->params)
				{
					const GpuParamDesc& paramsDesc = gpuParams->getParamDesc();

					if (staticBlockName != "")
					{
						auto findIter = paramsDesc.paramBlocks.find(staticBlockName);
						if (findIter != paramsDesc.paramBlocks.end())
						{
							// TODO - We only compare block sizes but not actual contents. Should I check them too?
							//        Probably shouldn't concern myself with that here, instead check that on a higher level.
							if (findIter->second.blockSize == mLitTexturedData.staticParamBlockDesc.blockSize)
							{
								UINT32 slotIdx = findIter->second.slot;
								element->material->rendererBuffers.push_back(MaterialProxy::BufferBindInfo(idx, slotIdx, mLitTexturedData.staticParamBuffer));
							}
						}
					}
					
					if (perFrameBlockName != "")
					{
						auto findIter = paramsDesc.paramBlocks.find(perFrameBlockName);
						if (findIter != paramsDesc.paramBlocks.end())
						{
							if (findIter->second.blockSize == mLitTexturedData.perFrameParamBlockDesc.blockSize)
							{
								UINT32 slotIdx = findIter->second.slot;
								element->material->rendererBuffers.push_back(MaterialProxy::BufferBindInfo(idx, slotIdx, mLitTexturedData.perFrameParamBuffer));
							}
						}
					}

					if (perObjectBlockName != "")
					{
						auto findIter = paramsDesc.paramBlocks.find(perObjectBlockName);
						if (findIter != paramsDesc.paramBlocks.end())
						{
							if (findIter->second.blockSize == mLitTexturedData.perObjectParamBlockDesc.blockSize)
							{
								if (rendererData->perObjectParamBuffer == nullptr)
									rendererData->perObjectParamBuffer = HardwareBufferManager::instance().createGpuParamBlockBuffer(mLitTexturedData.perObjectParamBlockDesc.blockSize);

								rendererData->perObjectBuffers.push_back(MaterialProxy::BufferBindInfo(idx, findIter->second.slot, rendererData->perObjectParamBuffer));

								if (!rendererData->hasWVPParam && wvpParamName != "")
								{
									auto findIter2 = paramsDesc.params.find(wvpParamName);
									if (findIter2 != paramsDesc.params.end())
									{
										if (paramsMatch(findIter2->second, mLitTexturedData.wvpParamDesc))
										{
											gpuParams->getParam(wvpParamName, rendererData->wvpParam);
											rendererData->hasWVPParam = true;
										}
									}
								}
							}
						}
					}

					idx++;
				}

				bindGlobalBuffers(element);
			}
		}
	}

	void BansheeRenderer::removeRenderableProxy(RenderableProxyPtr proxy)
	{
		for (auto& element : proxy->renderableElements)
		{
			assert(mRenderableElements.size() > element->id && element->id >= 0);

			if (mRenderableElements.size() == 0)
				mRenderableElements.erase(mRenderableElements.begin());
			else
			{
				std::swap(mRenderableElements[element->id], mRenderableElements.back());
				mRenderableElements.erase(mRenderableElements.end() - 1);

				mRenderableElements[element->id]->id = element->id;
			}
		}
	}

	void BansheeRenderer::updateRenderableProxy(RenderableProxyPtr proxy, Matrix4 localToWorld)
	{
		for (auto& element : proxy->renderableElements)
		{
			element->worldTransform = localToWorld;

			mWorldTransforms[element->id] = localToWorld;
			mWorldBounds[element->id] = element->calculateWorldBounds();
		}
	}

	void BansheeRenderer::addCameraProxy(CameraProxyPtr proxy)
	{
		RenderTargetPtr renderTarget = proxy->viewport.getTarget();
		auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });

		if (findIter != mRenderTargets.end())
		{
			proxy->renderQueue = bs_shared_ptr<RenderQueue>();

			findIter->cameras.push_back(proxy);
		}
		else
		{
			mRenderTargets.push_back(RenderTargetData());
			RenderTargetData& renderTargetData = mRenderTargets.back();

			proxy->renderQueue = bs_shared_ptr<RenderQueue>();
			renderTargetData.cameras.push_back(proxy);
		}

		// Sort everything based on priority
		auto cameraComparer = [&](const CameraProxyPtr& a, const CameraProxyPtr& b) { return a->priority > b->priority; };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b) { return a.target->getPriority() > b.target->getPriority(); };
		std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : mRenderTargets)
		{
			Vector<CameraProxyPtr>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}
	}

	void BansheeRenderer::removeCameraProxy(CameraProxyPtr proxy)
	{
		RenderTargetPtr renderTarget = proxy->viewport.getTarget();
		auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });

		if (findIter != mRenderTargets.end())
		{
			auto findIter2 = std::find(findIter->cameras.begin(), findIter->cameras.end(), proxy);

			if (findIter2 != findIter->cameras.end())
			{
				findIter->cameras.erase(findIter2);
			}

			if (findIter->cameras.size() == 0)
				mRenderTargets.erase(findIter);
		}
	}

	void BansheeRenderer::updateCameraProxy(CameraProxyPtr proxy, Matrix4 viewMatrix)
	{
		proxy->viewMatrix = viewMatrix;
	}

	void BansheeRenderer::renderableRemoved(const HRenderable& renderable)
	{
		if (renderable->_getActiveProxy() != nullptr)
		{
			mDeletedRenderableProxies.push_back(renderable->_getActiveProxy());
		}
	}

	void BansheeRenderer::cameraRemoved(const HCamera& camera)
	{
		if (camera->_getActiveProxy() != nullptr)
		{
			mDeletedCameraProxies.push_back(camera->_getActiveProxy());
		}
	}

	void BansheeRenderer::renderAll() 
	{
		gBsSceneManager().updateRenderableTransforms();

		// Remove proxies from deleted Renderables
		for (auto& proxy : mDeletedRenderableProxies)
		{
			if (proxy != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));
		}

		// Add or update Renderable proxies
		const Vector<HRenderable>& allRenderables = gBsSceneManager().getAllRenderables();
		Vector<HSceneObject> dirtySceneObjects;
		Vector<HRenderable> dirtyRenderables;

		for (auto& renderable : allRenderables)
		{
			bool addedNewProxy = false;
			RenderableProxyPtr proxy;

			if (!renderable->_isCoreDirty())
			{
				proxy = renderable->_getActiveProxy();

				if (proxy != nullptr)
					gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeRenderableProxy, this, proxy));

				proxy = renderable->_createProxy();
				renderable->_setActiveProxy(proxy);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addRenderableProxy, this, proxy));

				dirtyRenderables.push_back(renderable);
				dirtySceneObjects.push_back(renderable->SO());
				addedNewProxy = true;
			}
			else if (!renderable->SO()->_isCoreDirty())
			{
				proxy = renderable->_getActiveProxy();
				assert(proxy != nullptr);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateRenderableProxy, this, proxy, renderable->SO()->getWorldTfrm()));

				dirtySceneObjects.push_back(renderable->SO());
			}

			if (!addedNewProxy)
			{
				for (UINT32 i = 0; i < (UINT32)proxy->renderableElements.size(); i++)
				{
					HMaterial mat = renderable->getMaterial(i);
					if (mat != nullptr && mat.isLoaded() && mat->_isCoreDirty(MaterialDirtyFlag::Params))
					{
						gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateMaterialProxy, this, proxy->renderableElements[i]->material, mat->_getDirtyProxyParams()));
						mat->_markCoreClean(MaterialDirtyFlag::Params);
					}
				}
			}
		}

		// Mark all renderables as clean (needs to be done after all proxies are updated as
		// this will also clean materials & meshes which may be shared, so we don't want to clean them
		// too early.
		for (auto& renderable : dirtyRenderables)
		{
			renderable->_markCoreClean();
		}

		// Remove proxies from deleted Cameras
		for (auto& proxy : mDeletedCameraProxies)
		{
			if (proxy != nullptr)
				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeCameraProxy, this, proxy));
		}

		// Add or update Camera proxies
		const Vector<HCamera>& allCameras = gBsSceneManager().getAllCameras();
		for (auto& camera : allCameras)
		{
			if (!camera->_isCoreDirty())
			{
				CameraProxyPtr proxy = camera->_getActiveProxy();

				if (proxy != nullptr)
					gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::removeCameraProxy, this, proxy));

				proxy = camera->_createProxy();
				camera->_setActiveProxy(proxy);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addCameraProxy, this, proxy));

				camera->_markCoreClean();
				dirtySceneObjects.push_back(camera->SO());
			}
			else if (!camera->SO()->_isCoreDirty())
			{
				CameraProxyPtr proxy = camera->_getActiveProxy();
				assert(proxy != nullptr);

				gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::updateCameraProxy, this, proxy, camera->getViewMatrix()));

				dirtySceneObjects.push_back(camera->SO());
			}
		}

		// Mark scene objects clean
		for (auto& dirtySO : dirtySceneObjects)
		{
			dirtySO->_markCoreClean();
		}

		// Populate direct draw lists
		for (auto& camera : allCameras)
		{
			DrawListPtr drawList = bs_shared_ptr<DrawList>();

			// Get GUI render operations
			GUIManager::instance().render(camera->getViewport(), *drawList);

			// Get overlay render operations
			OverlayManager::instance().render(camera->getViewport(), *drawList);

			// Get debug render operations
			DrawHelper3D::instance().render(camera, *drawList);
			DrawHelper2D::instance().render(camera, *drawList);

			// Get any operations from hooked up callbacks
			const Viewport* viewportRawPtr = camera->getViewport().get();
			auto callbacksForViewport = mRenderCallbacks[viewportRawPtr];

			for (auto& callback : callbacksForViewport)
				callback(viewportRawPtr, *drawList);

			RenderQueuePtr renderQueue = bs_shared_ptr<RenderQueue>();
			const Vector<DrawOperation>& drawOps = drawList->getDrawOperations();
			for (auto& drawOp : drawOps)
			{
				if (drawOp.material->_isCoreDirty(MaterialDirtyFlag::Proxy))
					drawOp.material->_setActiveProxy(drawOp.material->_createProxy());

				if (drawOp.mesh->_isCoreDirty(MeshDirtyFlag::Proxy))
					drawOp.mesh->_setActiveProxy(drawOp.submeshIdx, drawOp.mesh->_createProxy(drawOp.submeshIdx));

				MaterialProxyPtr materialProxy = drawOp.material->_getActiveProxy();
				MeshProxyPtr meshProxy = drawOp.mesh->_getActiveProxy(drawOp.submeshIdx);

				renderQueue->add(materialProxy, meshProxy, drawOp.worldPosition);
			}

			gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addToRenderQueue, this, camera->_getActiveProxy(), drawList));
		}

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::renderAllCore, this, gTime().getTime()));
	}

	void BansheeRenderer::addToRenderQueue(CameraProxyPtr proxy, RenderQueuePtr renderQueue)
	{
		RenderQueuePtr cameraRenderQueue = proxy->renderQueue;

		const Vector<RenderQueueElement>& queueElements = renderQueue->getSortedElements();
		for (auto& queueElement : queueElements)
		{
			cameraRenderQueue->add(queueElement.material, queueElement.mesh, queueElement.worldPosition);
		}
	}

	void BansheeRenderer::updateMaterialProxy(MaterialProxyPtr proxy, Vector<MaterialProxy::ParamsBindInfo> dirtyParams)
	{
		for (UINT32 i = 0; i < (UINT32)dirtyParams.size(); i++)
			proxy->params[dirtyParams[i].paramsIdx] = dirtyParams[i].params;

		for (auto& rendererBuffer : proxy->rendererBuffers)
			proxy->params[rendererBuffer.paramsIdx]->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
	}

	void BansheeRenderer::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Update global hardware buffers
		mLitTexturedData.timeParam.set(time);

		mLitTexturedData.staticParams->updateHardwareBuffers();
		mLitTexturedData.perFrameParams->updateHardwareBuffers();

		// Render everything, target by target
		for (auto& renderTargetData : mRenderTargets)
		{
			RenderTargetPtr target = renderTargetData.target;
			Vector<CameraProxyPtr>& cameras = renderTargetData.cameras;

			RenderSystem::instance().beginFrame();

			for(auto& camera : cameras)
			{
				Viewport& viewport = camera->viewport;
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

				render(*camera, camera->renderQueue);
			}

			RenderSystem::instance().endFrame();
			RenderSystem::instance().swapBuffers(target);
		}
	}

	void BansheeRenderer::render(const CameraProxy& cameraProxy, const RenderQueuePtr& renderQueue) 
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		Matrix4 projMatrixCstm = cameraProxy.projMatrix;
		Matrix4 viewMatrixCstm = cameraProxy.viewMatrix;

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		if (!cameraProxy.ignoreSceneRenderables)
		{
			// Update per-object param buffers and queue render elements
			for (auto& proxy : mRenderableProxies)
			{
				for (auto& renderElem : proxy.second->renderableElements)
				{
					if (proxy.second->renderableType == RenType_LitTextured)
					{
						RenderLitTexturedPerObjectData* rendererData = any_cast_unsafe<RenderLitTexturedPerObjectData>(&renderElem->rendererData);

						if (rendererData->hasWVPParam)
						{
							Matrix4 worldViewProjMatrix = viewProjMatrix * renderElem->worldTransform;
							rendererData->wvpParam.set(worldViewProjMatrix);
						}

						if (rendererData->perObjectParamBuffer != nullptr)
						{
							GpuParamBlockPtr paramBlock = rendererData->perObjectParamBuffer->getParamBlock();
							if (paramBlock->isDirty())
								paramBlock->uploadToBuffer(rendererData->perObjectParamBuffer);
						}
					}

					for (auto& param : renderElem->material->params)
					{
						param->updateHardwareBuffers();
					}

					// TODO - Do frustum culling
					renderQueue->add(proxy.second, renderElem, mWorldBounds[renderElem->id].getSphere().getCenter());
				}
			}
		}

		renderQueue->sort();
		const Vector<RenderQueueElement>& sortedRenderElements = renderQueue->getSortedElements();

		for(auto iter = sortedRenderElements.begin(); iter != sortedRenderElements.end(); ++iter)
		{
			MaterialProxyPtr materialProxy = iter->renderElem->material;

			if (iter->renderable != nullptr)
				bindPerObjectBuffers(iter->renderable, iter->renderElem);

			setPass(materialProxy, iter->passIdx);
			draw(*iter->renderElem->mesh);
		}
	}

	void BansheeRenderer::bindGlobalBuffers(const RenderableElement* element)
	{
		for (auto& rendererBuffer : element->material->rendererBuffers)
		{
			element->material->params[rendererBuffer.paramsIdx]->setParamBlockBuffer(rendererBuffer.slotIdx, rendererBuffer.buffer);
		}
	}

	void BansheeRenderer::bindPerObjectBuffers(const RenderableProxyPtr& renderable, const RenderableElement* element)
	{
		if (renderable->renderableType != RenType_LitTextured)
			return;

		const RenderLitTexturedPerObjectData* rendererData = any_cast_unsafe<RenderLitTexturedPerObjectData>(&element->rendererData);
		for (auto& perObjectBuffer : rendererData->perObjectBuffers)
		{
			GpuParamsPtr params = element->material->params[perObjectBuffer.paramsIdx];

			params->setParamBlockBuffer(perObjectBuffer.slotIdx, rendererData->perObjectParamBuffer);
		}
	}

	void BansheeRenderer::setPass(const MaterialProxyPtr& material, UINT32 passIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();

		const MaterialProxyPass& pass = material->passes[passIdx];
		if (pass.vertexProg)
		{
			rs.bindGpuProgram(pass.vertexProg);
			rs.bindGpuParams(GPT_VERTEX_PROGRAM, material->params[pass.vertexProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_VERTEX_PROGRAM);

		if (pass.fragmentProg)
		{
			rs.bindGpuProgram(pass.fragmentProg);
			rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, material->params[pass.geometryProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_FRAGMENT_PROGRAM);

		if (pass.geometryProg)
		{
			rs.bindGpuProgram(pass.geometryProg);
			rs.bindGpuParams(GPT_GEOMETRY_PROGRAM, material->params[pass.geometryProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

		if (pass.hullProg)
		{
			rs.bindGpuProgram(pass.hullProg);
			rs.bindGpuParams(GPT_HULL_PROGRAM, material->params[pass.hullProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_HULL_PROGRAM);

		if (pass.domainProg)
		{
			rs.bindGpuProgram(pass.domainProg);
			rs.bindGpuParams(GPT_DOMAIN_PROGRAM, material->params[pass.domainProgParamsIdx]);
		}
		else
			rs.unbindGpuProgram(GPT_DOMAIN_PROGRAM);

		if (pass.computeProg)
		{
			rs.bindGpuProgram(pass.computeProg);
			rs.bindGpuParams(GPT_COMPUTE_PROGRAM, material->params[pass.computeProgParamsIdx]);
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

	void BansheeRenderer::draw(const MeshProxy& meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderSystem& rs = RenderSystem::instance();
		MeshBasePtr mesh = meshProxy.mesh;
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
		rs.drawIndexed(subMesh.indexOffset, indexCount, mesh->_getVertexOffset(), vertexData->vertexCount);

		mesh->_notifyUsedOnGPU();
	}
}