#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class GizmoManagerCore;

	class BS_ED_EXPORT GizmoManager : public Module<GizmoManager>
	{
	public:
		GizmoManager();
		~GizmoManager();

		void startGizmo(const HSceneObject& gizmoParent);
		void endGizmo();

		void setColor(const Color& color);
		void setTransform(const Matrix4& transform);
		void setPickable(bool pickable) { mPickable = pickable; }

		void drawCube(const Vector3& position, const Vector3& extents);
		void drawSphere(const Vector3& position, float radius);
		void drawWireCube(const Vector3& position, const Vector3& extents);
		void drawWireSphere(const Vector3& position, float radius);
		void drawLine(const Vector3& start, const Vector3& end);
		void drawFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far);
		void drawIcon(Vector3 position, HSpriteTexture image, bool fixedScale);

		void update();
		void renderForPicking(const HCamera& camera, std::function<Color(UINT32)> idxToColorCallback);
		void clearGizmos();

		HSceneObject getSceneObject(UINT32 gizmoIdx);

	private:
		friend class GizmoManagerCore;

		enum class GizmoMaterial
		{
			Solid, Wire, Picking
		};

		struct CommonData
		{
			UINT32 idx;
			Color color;
			Matrix4 transform;
			HSceneObject sceneObject;
			bool pickable;
		};

		struct CubeData : public CommonData
		{
			Vector3 position;
			Vector3 extents;
		};

		struct SphereData : public CommonData
		{
			Vector3 position;
			float radius;
		};

		struct LineData : public CommonData
		{
			Vector3 start;
			Vector3 end;
		};

		struct FrustumData : public CommonData
		{
			Vector3 position;
			float aspect;
			Degree FOV;
			float near;
			float far;
		};

		struct IconData : public CommonData
		{
			Vector3 position;
			bool fixedScale;
			HSpriteTexture texture;
		};

		struct IconRenderData
		{
			UINT32 count;
			HTexture texture;
		};

		struct CoreInitData
		{
			MaterialProxyPtr solidMatProxy;
			MaterialProxyPtr wireMatProxy;
			MaterialProxyPtr iconMatProxy;
			MaterialProxyPtr pickingMatProxy;
			MaterialProxyPtr alphaPickingMatProxy;
		};

		typedef Vector<IconRenderData> IconRenderDataVec;
		typedef std::shared_ptr<IconRenderDataVec> IconRenderDataVecPtr;

		TransientMeshPtr buildIconMesh(const HCamera& camera, const Vector<IconData>& iconData, bool forPicking, IconRenderDataVecPtr& renderData);

		void limitIconSize(UINT32& width, UINT32& height);
		void calculateIconColors(const Color& tint, const Camera& camera, UINT32 iconHeight, bool fixedScale,
			Color& normalColor, Color& fadedColor);

		void initializeCore(const CoreInitData& initData);
		void destroyCore(GizmoManagerCore* core);

		static const UINT32 VERTEX_BUFFER_GROWTH;
		static const UINT32 INDEX_BUFFER_GROWTH;
		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const float MAX_ICON_RANGE;
		static const UINT32 OPTIMAL_ICON_SIZE;
		static const float ICON_TEXEL_WORLD_SIZE;

		typedef Set<IconData, std::function<bool(const IconData&, const IconData&)>> IconSet;

		Color mColor;
		Matrix4 mTransform;
		HSceneObject mActiveSO;
		bool mPickable;
		UINT32 mCurrentIdx;

		DrawHelper* mDrawHelper;
		DrawHelper* mPickingDrawHelper;

		Vector<CubeData> mSolidCubeData;
		Vector<CubeData> mWireCubeData;
		Vector<SphereData> mSolidSphereData;
		Vector<SphereData> mWireSphereData;
		Vector<LineData> mLineData;
		Vector<FrustumData> mFrustumData;
		Vector<IconData> mIconData;
		Map<UINT32, HSceneObject> mIdxToSceneObjectMap;

		MeshHeapPtr mIconMeshHeap;

		TransientMeshPtr mIconMesh;

		GizmoManagerCore* mCore;

		// Immutable
		VertexDataDescPtr mIconVertexDesc;

		// Transient
		struct SortedIconData
		{
			float distance;
			Vector2I screenPosition;
			UINT32 iconIdx;
		};

		Vector<SortedIconData> mSortedIconData;
	};

	class GizmoManagerCore
	{
		friend class GizmoManager;

		struct SolidMaterialData
		{
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
		};

		struct WireMaterialData
		{
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
		};

		struct IconMaterialData
		{
			MaterialProxyPtr proxy;
			GpuParamsPtr mFragParams[2];
			GpuParamMat4 mViewProj[2];
			GpuParamTexture mTexture[2];
		};

		struct PickingMaterialData
		{
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
		};

		struct AlphaPickingMaterialData
		{
			MaterialProxyPtr proxy;
			GpuParamsPtr mFragParams;
			GpuParamMat4 mViewProj;
			GpuParamTexture mTexture;
		};

		struct PrivatelyConstuct { };

	public:
		GizmoManagerCore(const PrivatelyConstuct& dummy);

	private:
		void initialize(const GizmoManager::CoreInitData& initData);

		void render(const CameraProxy& camera);
		void renderGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr mesh, GizmoManager::GizmoMaterial material);
		void renderIconGizmos(Rect2I screenArea, MeshProxyPtr mesh, GizmoManager::IconRenderDataVecPtr renderData, bool usePickingMaterial);

		void updateData(const RenderTargetPtr& rt, const MeshProxyPtr& solidMeshProxy, const MeshProxyPtr& wireMeshProxy,
			const MeshProxyPtr& iconMeshProxy, const GizmoManager::IconRenderDataVecPtr& iconRenderData);

		static const float PICKING_ALPHA_CUTOFF;

		RenderTargetPtr mSceneRenderTarget;

		MeshProxyPtr mSolidMeshProxy;
		MeshProxyPtr mWireMeshProxy;
		MeshProxyPtr mIconMeshProxy;
		GizmoManager::IconRenderDataVecPtr mIconRenderData;

		// Immutable
		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;
		IconMaterialData mIconMaterial;

		PickingMaterialData mPickingMaterial;
		AlphaPickingMaterialData mAlphaPickingMaterial;
	};
}