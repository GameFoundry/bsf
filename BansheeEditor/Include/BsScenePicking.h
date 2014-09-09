#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class PickResult
	{
	public:
		enum class Type
		{
			SceneObject,
			Gizmo,
			None
		};

		HSceneObject sceneObject;
		UINT32 gizmoId;
		Type type;
	};

	class ScenePicking : public Module<ScenePicking>
	{
		struct RenderablePickData
		{
			MeshProxyPtr mesh;
			UINT32 index;
			Matrix4 wvpTransform;
			bool alpha;
			CullingMode cullMode;
		};

		struct MaterialData
		{
			// Sim thread
			HMaterial mMatPicking;
			HMaterial mMatPickingAlpha;

			// Core thread
			MaterialProxyPtr mMatPickingProxy;
			MaterialProxyPtr mMatPickingAlphaProxy;

			GpuParamMat4 mParamPickingWVP;
			GpuParamMat4 mParamPickingAlphaWVP;
			GpuParamColor mParamPickingColor;
			GpuParamColor mParamPickingAlphaColor;
			GpuParamTexture mParamPickingAlphaTexture;
		};

	public:
		ScenePicking();

		HSceneObject pickClosestSceneObject(const HCamera& cam, const Vector2I& position, const Vector2I& area);
		PickResult pickClosestObject(const HCamera& cam, const Vector2I& position, const Vector2I& area);
		Vector<HSceneObject> pickSceneObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area);
		Vector<PickResult> pickObjects(const HCamera& cam, const Vector2I& position, const Vector2I& area);

	private:
		typedef Set<RenderablePickData, std::function<bool(const RenderablePickData&, const RenderablePickData&)>> RenderableSet;

		void initializeCore();

		Color encodeIndex(UINT32 index);
		UINT32 decodeIndex(Color color);

		void corePickObjects(const Viewport& viewport, const RenderableSet& renderables, const Vector2I& position,
			const Vector2I& area, AsyncOp& asyncOp);

		static const float ALPHA_CUTOFF;

		MaterialData mMaterialData[3];
	};
}