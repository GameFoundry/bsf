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
#include "BsBansheeLitTexRenderableController.h"
#include "BsTime.h"
#include "BsRenderableElement.h"
#include "BsFrameAlloc.h"
#include "BsCoreObjectManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const String& BansheeRenderer::getName() const
	{
		static String name = "BansheeRenderer";
		return name;
	}

	void BansheeRenderer::_onActivated()
	{
		CoreRenderer::_onActivated();

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::initializeCore, this));
	}

	void BansheeRenderer::_onDeactivated()
	{
		CoreRenderer::_onDeactivated();

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::destroyCore, this));
		gCoreAccessor().submitToCoreThread(true);
	}

	void BansheeRenderer::initializeCore()
	{
		mLitTexHandler = bs_new<LitTexRenderableController>();

		SPtr<ShaderCore> shader = createDefaultShader();
		mDummyMaterial = MaterialCore::create(shader);
	}

	void BansheeRenderer::destroyCore()
	{
		if (mLitTexHandler != nullptr)
			bs_delete(mLitTexHandler);

		mRenderTargets.clear();
		mCameraData.clear();
		mRenderables.clear();

		mDummyMaterial = nullptr;
	}

	void BansheeRenderer::_notifyRenderableAdded(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = (UINT32)mRenderables.size();

		renderable->setRendererId(renderableId);

		mRenderables.push_back(RenderableData());
		mWorldTransforms.push_back(renderable->getTransform());
		mWorldBounds.push_back(renderable->getBounds());

		RenderableData& renderableData = mRenderables.back();
		renderableData.renderable = renderable;

		if (renderable->getRenderableType() == RenType_LitTextured)
			renderableData.controller = mLitTexHandler;
		else
			renderableData.controller = nullptr;

		SPtr<MeshCore> mesh = renderable->getMesh();
		if (mesh != nullptr)
		{
			const MeshProperties& meshProps = mesh->getProperties();
			for (UINT32 i = 0; i < meshProps.getNumSubMeshes(); i++)
			{
				renderableData.elements.push_back(RenderableElement());
				RenderableElement& renElement = renderableData.elements.back();

				renElement.mesh = mesh;
				renElement.subMesh = meshProps.getSubMesh(i);

				renElement.material = renderable->getMaterial(i);
				if (renElement.material == nullptr)
					renElement.material = renderable->getMaterial(0);

				if (renElement.material == nullptr)
					renElement.material = mDummyMaterial;

				if (renderableData.controller != nullptr)
					renderableData.controller->initializeRenderElem(renElement);
			}
		}
	}

	void BansheeRenderer::_notifyRenderableRemoved(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();
		RenderableHandlerCore* lastRenerable = mRenderables.back().renderable;
		UINT32 lastRenderableId = lastRenerable->getRendererId();

		if (renderableId != lastRenderableId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mRenderables[renderableId], mRenderables[lastRenderableId]);
			std::swap(mWorldBounds[renderableId], mWorldBounds[lastRenderableId]);
			std::swap(mWorldTransforms[renderableId], mWorldTransforms[lastRenderableId]);

			lastRenerable->setRendererId(renderableId);
		}

		// Last element is the one we want to erase
		mRenderables.erase(mRenderables.end() - 1);
		mWorldBounds.erase(mWorldBounds.end() - 1);
		mWorldTransforms.erase(mWorldTransforms.end() - 1);
	}

	void BansheeRenderer::_notifyRenderableUpdated(RenderableHandlerCore* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();

		mWorldTransforms[renderableId] = renderable->getTransform();
		mWorldBounds[renderableId] = renderable->getBounds();
	}

	void BansheeRenderer::_notifyCameraAdded(const CameraHandlerCore* camera)
	{
		CameraData& camData = mCameraData[camera];
		camData.renderQueue = bs_shared_ptr<RenderQueue>();
	}

	void BansheeRenderer::_notifyCameraRemoved(const CameraHandlerCore* camera)
	{
		mCameraData.erase(camera);
	}

	void BansheeRenderer::renderAll() 
	{
		// Populate direct draw lists
		const Map<CameraHandler*, SceneCameraData>& allCameras = gSceneManager().getAllCameras();
		for (auto& cameraData : allCameras)
		{
			CameraHandlerPtr camera = cameraData.second.camera;
			HSceneObject cameraSO = cameraData.second.sceneObject;

			DrawListPtr drawList = bs_shared_ptr<DrawList>();

			// Get GUI render operations
			GUIManager::instance().render(camera->getViewport(), *drawList);

			// Get overlay render operations
			OverlayManager::instance().render(camera->getViewport(), *drawList);

			// Get any operations from hooked up callbacks
			const Viewport* viewportRawPtr = camera->getViewport().get();
			onRenderViewport(viewportRawPtr, *drawList);

			RenderQueuePtr renderQueue = bs_shared_ptr<RenderQueue>();
			const Vector<DrawOperation>& drawOps = drawList->getDrawOperations();

			for (auto& drawOp : drawOps)
			{
				SPtr<MaterialCore> materialCore = drawOp.material->getCore();
				SPtr<MeshCoreBase> meshCore = drawOp.mesh->getCore();
				SubMesh subMesh = meshCore->getProperties().getSubMesh(drawOp.submeshIdx);

				float distanceToCamera = (cameraSO->getPosition() - drawOp.worldPosition).length();
				renderQueue->add(materialCore, meshCore, subMesh, distanceToCamera);
			}

			gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::addToRenderQueue, this, camera->getCore(), renderQueue));
		}

		// Sync all dirty sim thread CoreObject data to core thread
		CoreObjectManager::instance().syncToCore(gCoreAccessor());

		gCoreAccessor().queueCommand(std::bind(&BansheeRenderer::renderAllCore, this, gTime().getTime()));
	}

	void BansheeRenderer::addToRenderQueue(const SPtr<CameraHandlerCore>& camera, RenderQueuePtr renderQueue)
	{
		RenderQueuePtr cameraRenderQueue = mCameraData[camera.get()].renderQueue;
		cameraRenderQueue->add(*renderQueue);
	}

	void BansheeRenderer::renderAllCore(float time)
	{
		THROW_IF_NOT_CORE_THREAD;

		// Update global hardware buffers
		mLitTexHandler->updateGlobalBuffers(time);

		// Sort cameras by render target
		for (auto& cameraData : mCameraData)
		{
			const CameraHandlerCore* camera = cameraData.first;
			SPtr<RenderTargetCore> renderTarget = camera->getViewport()->getTarget();

			if (renderTarget == nullptr)
				continue;

			auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(), [&](const RenderTargetData& x) { return x.target == renderTarget; });
			if (findIter != mRenderTargets.end())
			{
				findIter->cameras.push_back(camera);
			}
			else
			{
				mRenderTargets.push_back(RenderTargetData());
				RenderTargetData& renderTargetData = mRenderTargets.back();

				renderTargetData.target = renderTarget;
				renderTargetData.cameras.push_back(camera);
			}
		}

		// Sort everything based on priority
		auto cameraComparer = [&](const CameraHandlerCore* a, const CameraHandlerCore* b) { return a->getPriority() > b->getPriority(); };
		auto renderTargetInfoComparer = [&](const RenderTargetData& a, const RenderTargetData& b)
		{ return a.target->getProperties().getPriority() > b.target->getProperties().getPriority(); };
		std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

		for (auto& camerasPerTarget : mRenderTargets)
		{
			Vector<const CameraHandlerCore*>& cameras = camerasPerTarget.cameras;

			std::sort(begin(cameras), end(cameras), cameraComparer);
		}

		// Render everything, target by target
		for (auto& renderTargetData : mRenderTargets)
		{
			SPtr<RenderTargetCore> target = renderTargetData.target;
			Vector<const CameraHandlerCore*>& cameras = renderTargetData.cameras;

			RenderAPICore::instance().beginFrame();

			for(auto& camera : cameras)
			{
				SPtr<ViewportCore> viewport = camera->getViewport();
				RenderAPICore::instance().setRenderTarget(target);
				RenderAPICore::instance().setViewport(viewport->getNormArea());

				UINT32 clearBuffers = 0;
				if(viewport->getRequiresColorClear())
					clearBuffers |= FBT_COLOR;

				if(viewport->getRequiresDepthClear())
					clearBuffers |= FBT_DEPTH;

				if(viewport->getRequiresStencilClear())
					clearBuffers |= FBT_STENCIL;

				if(clearBuffers != 0)
					RenderAPICore::instance().clearViewport(clearBuffers, viewport->getClearColor(), viewport->getClearDepthValue(), viewport->getClearStencilValue());

				render(*camera, mCameraData[camera].renderQueue);
			}

			RenderAPICore::instance().endFrame();
			RenderAPICore::instance().swapBuffers(target);
		}

		mRenderTargets.clear();
	}

	void BansheeRenderer::render(const CameraHandlerCore& camera, const RenderQueuePtr& renderQueue)
	{
		THROW_IF_NOT_CORE_THREAD;

		RenderAPICore& rs = RenderAPICore::instance();

		Matrix4 projMatrixCstm = camera.getProjectionMatrixRS();
		Matrix4 viewMatrixCstm = camera.getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		onCorePreRenderViewport(camera);

		if (!camera.getIgnoreSceneRenderables())
		{
			ConvexVolume worldFrustum = camera.getWorldFrustum();

			// Update per-object param buffers and queue render elements
			for (auto& renderableData : mRenderables)
			{
				RenderableHandlerCore* renderable = renderableData.renderable;
				RenderableController* controller = renderableData.controller;
				UINT32 renderableType = renderable->getRenderableType();
				UINT32 rendererId = renderable->getRendererId();

				// Update buffers
				for (auto& renderElem : renderableData.elements)
				{
					if (controller != nullptr)
						controller->bindPerObjectBuffers(renderElem);

					if (renderableType == RenType_LitTextured)
					{
						Matrix4 worldViewProjMatrix = viewProjMatrix * mWorldTransforms[rendererId];
						mLitTexHandler->updatePerObjectBuffers(renderElem, worldViewProjMatrix);
					}

					UINT32 numPasses = renderElem.material->getNumPasses();
					for (UINT32 i = 0; i < numPasses; i++)
					{
						SPtr<PassParametersCore> passParams = renderElem.material->getPassParameters(i);

						for (UINT32 j = 0; j < passParams->getNumParams(); j++)
						{
							SPtr<GpuParamsCore> params = passParams->getParamByIdx(j);
							if (params != nullptr)
								params->updateHardwareBuffers();
						}
					}
				}

				// Do frustum culling
				// TODO - This is bound to be a bottleneck at some point. When it is ensure that intersect
				// methods use vector operations, as it is trivial to update them.
				const Sphere& boundingSphere = mWorldBounds[rendererId].getSphere();
				if (worldFrustum.intersects(boundingSphere))
				{
					// More precise with the box
					const AABox& boundingBox = mWorldBounds[rendererId].getBox();

					if (worldFrustum.intersects(boundingBox))
					{
						float distanceToCamera = (camera.getPosition() - boundingBox.getCenter()).length();

						for (auto& renderElem : renderableData.elements)
							renderQueue->add(&renderElem, distanceToCamera);
					}
				}
			}
		}

		renderQueue->sort();
		const Vector<RenderQueueElement>& sortedRenderElements = renderQueue->getSortedElements();

		for(auto iter = sortedRenderElements.begin(); iter != sortedRenderElements.end(); ++iter)
		{
			SPtr<MaterialCore> material = iter->material;

			setPass(material, iter->passIdx);
			draw(iter->mesh, iter->subMesh);
		}

		renderQueue->clear();

		onCorePostRenderViewport(camera);
	}

	SPtr<ShaderCore> BansheeRenderer::createDefaultShader()
	{
		String rsName = RenderAPICore::instance().getName();

		SPtr<GpuProgramCore> vsProgram;
		SPtr<GpuProgramCore> psProgram;

		if (rsName == RenderAPIDX11)
		{
			String vsCode = R"(
				cbuffer PerObject
				{
					float4x4 matWorldViewProj;
				}

				void vs_main(
				in float3 inPos : POSITION,
				out float4 oPosition : SV_Position)
				{
					oPosition = mul(matWorldViewProj, float4(inPos.xyz, 1));
				})";

			String psCode = R"(
				float4 ps_main() : SV_Target
				{
					return float4(0.3f, 0.9f, 0.3f, 1.0f);
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}
		else if (rsName == RenderAPIDX9)
		{
			String vsCode = R"(
				 BS_PARAM_BLOCK PerObject { matWorldViewProj }
				 float4x4 matWorldViewProj;

				 void vs_main(
				 in float3 inPos : POSITION,
				 out float4 oPosition : POSITION)
				 {
					 oPosition = mul(matWorldViewProj, float4(inPos.xyz, 1));
				 })";

			String psCode = R"(
				float4 ps_main() : COLOR0
				{
					return float4(0.3f, 0.9f, 0.3f, 1.0f);
				})";

			vsProgram = GpuProgramCore::create(vsCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
			psProgram = GpuProgramCore::create(psCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_FS_2_0);
		}
		else if (rsName == RenderAPIOpenGL)
		{
			String vsCode = R"(#version 400
				uniform PerObject
				{
					mat4 matWorldViewProj;
				};

				in vec3 bs_position;

				void main()
				{
					gl_Position = matWorldViewProj * vec4(bs_position.xyz, 1);
				})";

			String psCode = R"(#version 400
				out vec4 fragColor;

				void main()
				{
					fragColor = vec4(0.3f, 0.9f, 0.3f, 1.0f);
				})";

			vsProgram = GpuProgramCore::create(vsCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);
			psProgram = GpuProgramCore::create(psCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_FS_4_0);
		}

		PASS_DESC_CORE passDesc;
		passDesc.vertexProgram = vsProgram;
		passDesc.fragmentProgram = psProgram;

		SPtr<PassCore> newPass = PassCore::create(passDesc);
		SPtr<TechniqueCore> newTechnique = TechniqueCore::create(rsName, RendererDefault, { newPass });

		SHADER_DESC shaderDesc;
		shaderDesc.setParamBlockAttribs("PerObject", true, GPBU_DYNAMIC, RBS_PerObject);
		shaderDesc.addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4, RPS_WorldViewProjTfrm);

		SPtr<ShaderCore> defaultShader = ShaderCore::create("DummyShader", shaderDesc, { newTechnique });

		return defaultShader;
	}
}