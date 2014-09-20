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
				GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

				vertParams->getParam("matWorldViewProj", md.mParamPickingWVP);

				GpuParamsPtr fragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];

				fragParams->getParam("colorIndex", md.mParamPickingColor);
			}

			{
				MaterialProxyPtr proxy = md.mMatPickingAlphaProxy;
				GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

				vertParams->getParam("matWorldViewProj", md.mParamPickingAlphaWVP);

				GpuParamsPtr fragParams = proxy->params[proxy->passes[0].fragmentProgParamsIdx];

				fragParams->getParam("colorIndex", md.mParamPickingAlphaColor);
				fragParams->getTextureParam("mainTexture", md.mParamPickingAlphaTexture);

				GpuParamFloat alphaCutoffParam;
				fragParams->getParam("alphaCutoff", alphaCutoffParam);
				alphaCutoffParam.set(ALPHA_CUTOFF);
			}
		}
	}

	HSceneObject ScenePicking::pickClosestSceneObject(const HCamera& cam, const Vector2I& position, const Vector2I& area)
	{
		Vector<PickResult> selectedObjects = pickObjects(cam, position, area);
		for (auto& object : selectedObjects)
		{
			if (object.type == PickResult::Type::SceneObject)
				return object.sceneObject;
		}

		return HSceneObject();
	}

	PickResult ScenePicking::pickClosestObject(const HCamera& cam, const Vector2I& position, const Vector2I& area)
	{
		Vector<PickResult> selectedObjects = pickObjects(cam, position, area);
		if (selectedObjects.size() == 0)
			return { HSceneObject(), 0, PickResult::Type::None };

		return selectedObjects[0];
	}

	Vector<HSceneObject> ScenePicking::pickSceneObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area)
	{
		Vector<HSceneObject> results;
		Vector<PickResult> selectedObjects = pickObjects(cam, position, area);
		for (auto& object : selectedObjects)
		{
			if (object.type == PickResult::Type::SceneObject)
				results.push_back(object.sceneObject);
		}

		return results;
	}

	Vector<PickResult> ScenePicking::pickObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area)
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

		const Vector<HRenderable>& renderables = SceneManager::instance().getAllRenderables();
		RenderableSet pickData(comparePickElement);
		Map<UINT32, HRenderable> idxToRenderable;

		for (auto& renderable : renderables)
		{
			HSceneObject so = renderable->SO();

			if (!so->getActive())
				continue;

			HMesh mesh = renderable->getMesh();
			if (!mesh)
				continue;

			Bounds worldBounds = mesh->getBounds();
			Matrix4 worldTransform = so->getWorldTfrm();
			worldBounds.transformAffine(worldTransform);

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
						CullingMode cullMode = firstPass->getRasterizerState()->getCullMode();

						MeshProxyPtr meshProxy;

						if (mesh->_isCoreDirty(MeshDirtyFlag::Proxy))
							meshProxy = mesh->_createProxy(i);
						else
							meshProxy = mesh->_getActiveProxy(i);

						idxToRenderable[idx] = renderable;
						pickData.insert({ meshProxy, idx, worldTransform, useAlphaShader, cullMode });
					}
				}
			}
		}

		Viewport vp = cam->getViewport()->clone();
		AsyncOp op = gCoreAccessor().queueReturnCommand(std::bind(&ScenePicking::corePickObjects, this, vp, std::cref(pickData), position, area, _1));
		gCoreAccessor().submitToCoreThread(true);

		assert(op.hasCompleted());

		Vector<UINT32>& selectedObjects = op.getReturnValue<Vector<UINT32>>();
		Vector<PickResult> results;

		for (auto& selectedObject : selectedObjects)
		{
			results.push_back({ idxToRenderable[selectedObject]->SO(), 0, PickResult::Type::SceneObject });
		}

		return results;
	}

	void ScenePicking::corePickObjects(const Viewport& vp, const RenderableSet& renderables, const Vector2I& position,
		const Vector2I& area, AsyncOp& asyncOp)
	{
		RenderSystem& rs = RenderSystem::instance();

		RenderTargetPtr rt = vp.getTarget();
		if (rt->getCore()->getProperties().isWindow())
		{
			// TODO: When I do implement this then I will likely want a method in RenderTarget that unifies both render window and render texture readback
			BS_EXCEPT(NotImplementedException, "Picking is not supported on render windows as framebuffer readback methods aren't implemented");
		}

		RenderTexturePtr rtt = std::static_pointer_cast<RenderTexture>(rt);
		TexturePtr outputTexture = rtt->getBindableColorTexture().getInternalPtr();

		rs.setViewport(vp);
		rs.clearRenderTarget(FBT_COLOR, Color::White);
		rs.setScissorRect(position.x, position.y, position.x + area.x, position.y + area.y);

		Renderer::setPass(*mMaterialData[0].mMatPickingProxy, 0);
		bool activeMaterialIsAlpha = false;
		CullingMode activeMaterialCull = (CullingMode)0;

		rs.beginFrame();
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
				md.mParamPickingAlphaTexture.set(HTexture()); // TODO - Get main texture from original
			}
			else
			{
				md.mParamPickingWVP.set(renderable.wvpTransform);
				md.mParamPickingColor.set(color);
			}

			Renderer::draw(*renderable.mesh);
		}
		rs.endFrame();

		PixelDataPtr outputPixelData = outputTexture->allocateSubresourceBuffer(0);
		GpuResourceDataPtr outputData = outputPixelData;
		AsyncOp unused;

		rs.readSubresource(outputTexture, 0, outputData, unused);

		Map<UINT32, UINT32> selectionScores;
		UINT32 numPixels = outputPixelData->getWidth() * outputPixelData->getHeight();
		for (UINT32 y = 0; y < outputPixelData->getHeight(); y++)
		{
			for (UINT32 x = 0; x < outputPixelData->getWidth(); x++)
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

		// Sort by score
		struct SelectedObject { UINT32 index; UINT32 score; };

		Vector<SelectedObject> selectedObjects(selectionScores.size());
		UINT32 idx = 0;
		for (auto& selectionScore : selectionScores)
		{
			selectedObjects[idx++] = { selectionScore.second, selectionScore.first };
		}

		std::sort(selectedObjects.begin(), selectedObjects.end(),
			[&](const SelectedObject& a, const SelectedObject& b)
		{
			return b.score < a.score;
		});

		Vector<UINT32> results;
		for (auto& selectedObject : selectedObjects)
			results.push_back(selectedObject.index);

		asyncOp._completeOperation(selectedObjects);
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