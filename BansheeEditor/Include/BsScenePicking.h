#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class ScenePickingCore;

	class ScenePicking : public Module<ScenePicking>
	{
		struct RenderablePickData
		{
			SPtr<MeshCore> mesh;
			UINT32 index;
			Matrix4 wvpTransform;
			bool alpha;
			CullingMode cullMode;
			HTexture mainTexture;
		};

	public:
		ScenePicking();
		~ScenePicking();

		HSceneObject pickClosestObject(const CameraHandlerPtr& cam, const Vector2I& position, const Vector2I& area);
		Vector<HSceneObject> pickObjects(const CameraHandlerPtr& cam, const Vector2I& position, const Vector2I& area);

	private:
		friend class ScenePickingCore;

		typedef Set<RenderablePickData, std::function<bool(const RenderablePickData&, const RenderablePickData&)>> RenderableSet;

		static Color encodeIndex(UINT32 index);
		static UINT32 decodeIndex(Color color);

		ScenePickingCore* mCore;
	};

	class ScenePickingCore
	{
		struct MaterialData
		{
			SPtr<MaterialCore> mMatPickingCore;
			SPtr<MaterialCore> mMatPickingAlphaCore;

			SPtr<GpuParamsCore> mParamPickingVertParams;
			SPtr<GpuParamsCore> mParamPickingFragParams;
			SPtr<GpuParamsCore> mParamPickingAlphaVertParams;
			SPtr<GpuParamsCore> mParamPickingAlphaFragParams;

			GpuParamMat4Core mParamPickingWVP;
			GpuParamMat4Core mParamPickingAlphaWVP;
			GpuParamColorCore mParamPickingColor;
			GpuParamColorCore mParamPickingAlphaColor;
			GpuParamTextureCore mParamPickingAlphaTexture;
		};

	public:
		void initialize();
		void destroy();

		void corePickingBegin(const SPtr<RenderTargetCore>& target, const Rect2& viewportArea, const ScenePicking::RenderableSet& renderables,
			const Vector2I& position, const Vector2I& area);
		void corePickingEnd(const SPtr<RenderTargetCore>& target, const Rect2& viewportArea, const Vector2I& position,
			const Vector2I& area, AsyncOp& asyncOp);

	private:
		friend class ScenePicking;

		static const float ALPHA_CUTOFF;

		MaterialData mMaterialData[3];
	};
}