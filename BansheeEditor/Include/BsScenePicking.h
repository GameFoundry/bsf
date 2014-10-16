#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class ScenePicking : public Module<ScenePicking>
	{
		struct RenderablePickData
		{
			MeshProxyPtr mesh;
			UINT32 index;
			Matrix4 wvpTransform;
			bool alpha;
			CullingMode cullMode;
			HTexture mainTexture;
		};

		struct MaterialData
		{
			// Sim thread
			HMaterial mMatPicking;
			HMaterial mMatPickingAlpha;

			// Core thread
			MaterialProxyPtr mMatPickingProxy;
			MaterialProxyPtr mMatPickingAlphaProxy;

			GpuParamsPtr mParamPickingVertParams;
			GpuParamsPtr mParamPickingFragParams;
			GpuParamsPtr mParamPickingAlphaVertParams;
			GpuParamsPtr mParamPickingAlphaFragParams;

			GpuParamMat4 mParamPickingWVP;
			GpuParamMat4 mParamPickingAlphaWVP;
			GpuParamColor mParamPickingColor;
			GpuParamColor mParamPickingAlphaColor;
			GpuParamTexture mParamPickingAlphaTexture;
		};

	public:
		ScenePicking();

		HSceneObject pickClosestObject(const HCamera& cam, const Vector2I& position, const Vector2I& area);
		Vector<HSceneObject> pickObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area);

	private:
		typedef Set<RenderablePickData, std::function<bool(const RenderablePickData&, const RenderablePickData&)>> RenderableSet;

		void initializeCore();

		Color encodeIndex(UINT32 index);
		UINT32 decodeIndex(Color color);

		void corePickingBegin(const Viewport& viewport, const RenderableSet& renderables, const Vector2I& position, const Vector2I& area);
		void corePickingEnd(const Viewport& viewport, const Vector2I& position, const Vector2I& area, AsyncOp& asyncOp);

		static const float ALPHA_CUTOFF;

		MaterialData mMaterialData[3];
	};
}