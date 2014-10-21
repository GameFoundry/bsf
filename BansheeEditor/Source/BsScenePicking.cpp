#include "BsScenePicking.h"
#include "BsSceneManager.h"
#include "BsColor.h"
#include "BsMatrix4.h"
#include "BsDebug.h"
#include "BsMath.h"
#include "BsRenderable.h"
#include "BsSceneObject.h"
#include "BsMesh.h"
#include "BsConvexVolume.h"
#include "BsCamera.h"
#include "BsCoreThread.h"
#include "BsRenderSystem.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsRenderTarget.h"
#include "BsPixelData.h"
#include "BsGpuParams.h"
#include "BsBuiltinEditorResources.h"
#include "BsShader.h"
#include "BsRenderer.h"
#include "BsGizmoManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const float ScenePicking::ALPHA_CUTOFF = 0.5f;

	ScenePicking::ScenePicking()
	{
		for (UINT32 i = 0; i < 3; i++)
		{
			mMaterialData[i].mMatPicking = BuiltinEditorResources::instance().createPicking((CullingMode)i);
			mMaterialData[i].mMatPickingAlpha = BuiltinEditorResources::instance().createPickingAlpha((CullingMode)i);

			mMaterialData[i].mMatPickingProxy = mMaterialData[i].mMatPicking->_createProxy();
			mMaterialData[i].mMatPickingAlphaProxy = mMaterialData[i].mMatPickingAlpha->_createProxy();
		}

		gCoreAccessor().queueCommand(std::bind(&ScenePicking::initializeCore, this));
	}

	void ScenePicking::initializeCore()
	{
		for (UINT32 i = 0; i < 3; i++)
		{
			MaterialData& md = mMaterialData[i];

			{
				// TODO - Make a better interface when dealing with parameters through proxies?
				MaterialProxyPtr proxy = md.mMatPickingProxy;

				md.mParamPickingVertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];
				md.mParamPickingVertParams->getParam("matWorldViewProj", md.mParamPickingWVP);

				md.mParamPickingFragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];
				md.mParamPickingFragParams->getParam("colorIndex", md.mParamPickingColor);
			}

			{
				MaterialProxyPtr proxy = md.mMatPickingAlphaProxy;

				md.mParamPickingAlphaVertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];
				md.mParamPickingAlphaVertParams->getParam("matWorldViewProj", md.mParamPickingAlphaWVP);

				md.mParamPickingAlphaFragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];

				md.mParamPickingAlphaFragParams->getParam("colorIndex", md.mParamPickingAlphaColor);
				md.mParamPickingAlphaFragParams->getTextureParam("mainTexture", md.mParamPickingAlphaTexture);

				GpuParamFloat alphaCutoffParam;
				md.mParamPickingAlphaFragParams->getParam("alphaCutoff", alphaCutoffParam);
				alphaCutoffParam.set(ALPHA_CUTOFF);
			}
		}
	}

	HSceneObject ScenePicking::pickClosestObject(const HCamera& cam, const Vector2I& position, const Vector2I& area)
	{
		Vector<HSceneObject> selectedObjects = pickObjects(cam, position, area);
		if (selectedObjects.size() == 0)
			return HSceneObject();

		return selectedObjects[0];
	}

	Vector<HSceneObject> ScenePicking::pickObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area)
	{
		auto comparePickElement = [&] (const ScenePicking::RenderablePickData& a, const ScenePicking::RenderablePickData& b)
		{
			// Sort by alpha setting first, then by cull mode, then by index
			if (a.alpha == b.alpha)
			{
				if (a.cullMode == b.cullMode)
					return a.index > b.index;
				else
					return (UINT32)a.cullMode > (UINT32)b.cullMode;
			}
			else
				return (UINT32)a.alpha > (UINT32)b.alpha;
		};

		Matrix4 viewProjMatrix = cam->getProjectionMatrixRS() * cam->getViewMatrix();

		const Vector<SceneRenderableData>& renderables = SceneManager::instance().getAllRenderables();
		RenderableSet pickData(comparePickElement);
		Map<UINT32, HSceneObject> idxToRenderable;

		for (auto& renderableData : renderables)
		{
			RenderableHandlerPtr renderable = renderableData.renderable;
			HSceneObject so = renderableData.sceneObject;

			if (!so->getActive())
				continue;

			HMesh mesh = renderable->getMesh();
			if (!mesh)
				continue;

			Bounds worldBounds = mesh->getBounds();
			Matrix4 worldTransform = so->getWorldTfrm();
			worldBounds.transformAffine(worldTransform);

			// TODO - I could limit the frustum to the visible area we're rendering for a speed boost
			// but this is unlikely to be a performance bottleneck
			const ConvexVolume& frustum = cam->getWorldFrustum();
			if (frustum.intersects(worldBounds.getSphere()))
			{
				// More precise with the box
				if (frustum.intersects(worldBounds.getBox()))
				{
					for (UINT32 i = 0; i < mesh->getNumSubMeshes(); i++)
					{
						UINT32 idx = (UINT32)pickData.size();

						HMaterial originalMat = renderable->getMaterial(i);
						if (!originalMat)
							continue;

						PassPtr firstPass;
						if (originalMat->getNumPasses() == 0)
							continue;

						firstPass = originalMat->getPass(0); // Note: We only ever check the first pass, problem?
						bool useAlphaShader = firstPass->hasBlending();
						
						RasterizerStatePtr rasterizerState;
						if (firstPass->getRasterizerState() == nullptr)
							rasterizerState = RasterizerState::getDefault();
						else
							rasterizerState = firstPass->getRasterizerState().getInternalPtr();

						CullingMode cullMode = rasterizerState->getCullMode();

						MeshProxyPtr meshProxy;

						if (mesh->_isCoreDirty(MeshDirtyFlag::Proxy))
							meshProxy = mesh->_createProxy(i);
						else
							meshProxy = mesh->_getActiveProxy(i);

						HTexture mainTexture;
						if (useAlphaShader)
						{
							const Map<String, SHADER_OBJECT_PARAM_DESC>& objectParams = originalMat->getShader()->_getObjectParams();
							for (auto& objectParam : objectParams)
							{
								if (objectParam.second.rendererSemantic == RPS_MainTex)
								{
									mainTexture = originalMat->getTexture(objectParam.first);
									break;
								}
							}
						}

						idxToRenderable[idx] = so;

						Matrix4 wvpTransform = viewProjMatrix * worldTransform;
						pickData.insert({ meshProxy, idx, wvpTransform, useAlphaShader, cullMode, mainTexture });
					}
				}
			}
		}

		UINT32 firstGizmoIdx = (UINT32)pickData.size();

		Viewport vp = cam->getViewport()->clone();
		gCoreAccessor().queueCommand(std::bind(&ScenePicking::corePickingBegin, this, vp, std::cref(pickData), position, area));

		GizmoManager::instance().renderForPicking(cam, [&](UINT32 inputIdx) { return encodeIndex(firstGizmoIdx + inputIdx); });

		AsyncOp op = gCoreAccessor().queueReturnCommand(std::bind(&ScenePicking::corePickingEnd, this, vp, position, area, _1));
		gCoreAccessor().submitToCoreThread(true);

		assert(op.hasCompleted());

		Vector<UINT32>& selectedObjects = op.getReturnValue<Vector<UINT32>>();
		Vector<HSceneObject> results;

		for (auto& selectedObjectIdx : selectedObjects)
		{
			if (selectedObjectIdx < firstGizmoIdx)
			{
				auto iterFind = idxToRenderable.find(selectedObjectIdx);

				if (iterFind != idxToRenderable.end())
					results.push_back(iterFind->second);
			}
			else
			{
				UINT32 gizmoIdx = selectedObjectIdx - firstGizmoIdx;

				HSceneObject so = GizmoManager::instance().getSceneObject(gizmoIdx);
				if (so)
					results.push_back(so);
			}
		}

		return results;
	}

	void ScenePicking::corePickingBegin(const Viewport& viewport, const RenderableSet& renderables, const Vector2I& position, const Vector2I& area)
	{
		RenderSystem& rs = RenderSystem::instance();

		rs.beginFrame();
		rs.setViewport(viewport);
		rs.clearRenderTarget(FBT_COLOR | FBT_DEPTH | FBT_STENCIL, Color::White);
		rs.setScissorRect(position.x, position.y, position.x + area.x, position.y + area.y);

		Renderer::setPass(*mMaterialData[0].mMatPickingProxy, 0);
		bool activeMaterialIsAlpha = false;
		CullingMode activeMaterialCull = (CullingMode)0;

		for (auto& renderable : renderables)
		{
			if (activeMaterialIsAlpha != renderable.alpha || activeMaterialCull != renderable.cullMode)
			{
				activeMaterialIsAlpha = renderable.alpha;
				activeMaterialCull = renderable.cullMode;

				if (activeMaterialIsAlpha)
					Renderer::setPass(*mMaterialData[(UINT32)activeMaterialCull].mMatPickingAlphaProxy, 0);
				else
					Renderer::setPass(*mMaterialData[(UINT32)activeMaterialCull].mMatPickingProxy, 0);
			}

			Color color = encodeIndex(renderable.index);
			MaterialData& md = mMaterialData[(UINT32)activeMaterialCull];

			if (activeMaterialIsAlpha)
			{
				md.mParamPickingAlphaWVP.set(renderable.wvpTransform);
				md.mParamPickingAlphaColor.set(color);
				md.mParamPickingAlphaTexture.set(renderable.mainTexture);

				rs.bindGpuParams(GPT_VERTEX_PROGRAM, md.mParamPickingAlphaVertParams);
				rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, md.mParamPickingAlphaFragParams);
			}
			else
			{
				md.mParamPickingWVP.set(renderable.wvpTransform);
				md.mParamPickingColor.set(color);

				rs.bindGpuParams(GPT_VERTEX_PROGRAM, md.mParamPickingVertParams);
				rs.bindGpuParams(GPT_FRAGMENT_PROGRAM, md.mParamPickingFragParams);
			}

			Renderer::draw(*renderable.mesh);
		}
	}

	void ScenePicking::corePickingEnd(const Viewport& vp, const Vector2I& position, const Vector2I& area, AsyncOp& asyncOp)
	{
		RenderTargetPtr rt = vp.getTarget();
		if (rt->getCore()->getProperties().isWindow())
		{
			// TODO: When I do implement this then I will likely want a method in RenderTarget that unifies both render window and render texture readback
			BS_EXCEPT(NotImplementedException, "Picking is not supported on render windows as framebuffer readback methods aren't implemented");
		}

		RenderTexturePtr rtt = std::static_pointer_cast<RenderTexture>(rt);
		TexturePtr outputTexture = rtt->getBindableColorTexture().getInternalPtr();

		if (position.x < 0 || position.x >= (INT32)outputTexture->getWidth() ||
			position.y < 0 || position.y >= (INT32)outputTexture->getHeight())
		{
			asyncOp._completeOperation(Vector<UINT32>());
			return;
		}

		PixelDataPtr outputPixelData = outputTexture->allocateSubresourceBuffer(0);
		GpuResourceDataPtr outputData = outputPixelData;
		AsyncOp unused;

		RenderSystem& rs = RenderSystem::instance();

		rs.endFrame();
		rs.readSubresource(outputTexture, 0, outputData, unused);

		Map<UINT32, UINT32> selectionScores;
		UINT32 numPixels = outputPixelData->getWidth() * outputPixelData->getHeight();

		UINT32 maxWidth = std::min((UINT32)(position.x + area.x), outputPixelData->getWidth());
		UINT32 maxHeight = std::min((UINT32)(position.y + area.y), outputPixelData->getHeight());

		if (rt->requiresTextureFlipping())
		{
			UINT32 vertOffset = outputPixelData->getHeight() - 1;

			for (UINT32 y = maxHeight; y > (UINT32)position.y; y--)
			{
				for (UINT32 x = (UINT32)position.x; x < maxWidth; x++)
				{
					Color color = outputPixelData->getColorAt(x, vertOffset - y);
					UINT32 index = decodeIndex(color);

					if (index == 0x00FFFFFF) // Nothing selected
						continue;

					auto iterFind = selectionScores.find(index);
					if (iterFind == selectionScores.end())
						selectionScores[index] = 1;
					else
						iterFind->second++;
				}
			}
		}
		else
		{
			for (UINT32 y = (UINT32)position.y; y < maxHeight; y++)
			{
				for (UINT32 x = (UINT32)position.x; x < maxWidth; x++)
				{
					Color color = outputPixelData->getColorAt(x, y);
					UINT32 index = decodeIndex(color);

					if (index == 0x00FFFFFF) // Nothing selected
						continue;

					auto iterFind = selectionScores.find(index);
					if (iterFind == selectionScores.end())
						selectionScores[index] = 1;
					else
						iterFind->second++;
				}
			}
		}

		// Sort by score
		struct SelectedObject { UINT32 index; UINT32 score; };

		Vector<SelectedObject> selectedObjects(selectionScores.size());
		UINT32 idx = 0;
		for (auto& selectionScore : selectionScores)
		{
			selectedObjects[idx++] = { selectionScore.first, selectionScore.second };
		}

		std::sort(selectedObjects.begin(), selectedObjects.end(),
			[&](const SelectedObject& a, const SelectedObject& b)
		{
			return b.score < a.score;
		});

		Vector<UINT32> results;
		for (auto& selectedObject : selectedObjects)
			results.push_back(selectedObject.index);

		asyncOp._completeOperation(results);
	}

	Color ScenePicking::encodeIndex(UINT32 index)
	{
		Color encoded;
		encoded.r = (index & 0xFF) / 255.0f;
		encoded.g = ((index >> 8) & 0xFF) / 255.0f;
		encoded.b = ((index >> 16) & 0xFF) / 255.0f;
		encoded.a = 1.0f;

		if (((index >> 24) & 0xFF))
			LOGERR("Index when picking out of valid range.");

		return encoded;
	}

	UINT32 ScenePicking::decodeIndex(Color color)
	{
		UINT32 r = Math::roundToInt(color.r * 255.0f);
		UINT32 g = Math::roundToInt(color.g * 255.0f);
		UINT32 b = Math::roundToInt(color.b * 255.0f);

		return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16);
	}
}