#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsVector2I.h"
#include "BsMatrix4.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GizmoManager : public Module<GizmoManager>
	{
	public:
		GizmoManager(const HCamera& camera);
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
		void clearGizmos();

	private:
		struct CommonData
		{
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

		struct SolidMaterialData
		{
			HMaterial material;
			
			// Core
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
			GpuParamMat4 mViewIT;
		};

		struct WireMaterialData
		{
			HMaterial material;

			// Core
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
		};

		struct IconMaterialData
		{
			HMaterial material;

			// Core
			MaterialProxyPtr proxy;
			GpuParamMat4 mViewProj;
			GpuParamTexture mTexture;
		};

		typedef Vector<IconRenderData> IconRenderDataVec;
		typedef std::shared_ptr<IconRenderDataVec> IconRenderDataVecPtr;

		void buildSolidMesh();
		void buildWireMesh();
		IconRenderDataVecPtr buildIconMesh();

		void coreRenderSolidGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr mesh);
		void coreRenderWireGizmos(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr mesh);
		void coreRenderIconGizmos(RectI screenArea, MeshProxyPtr mesh, IconRenderDataVecPtr renderData);

		void limitIconSize(UINT32& width, UINT32& height);
		void calculateIconColors(const Color& tint, const Camera& camera, UINT32 iconHeight, bool fixedScale,
			Color& normalColor, Color& fadedColor);

		void initializeCore();

		static const UINT32 VERTEX_BUFFER_GROWTH;
		static const UINT32 INDEX_BUFFER_GROWTH;
		static const UINT32 SPHERE_QUALITY;
		static const float MAX_ICON_RANGE;
		static const UINT32 OPTIMAL_ICON_SIZE;
		static const float ICON_TEXEL_WORLD_SIZE;

		typedef Set<IconData, std::function<bool(const IconData&, const IconData&)>> IconSet;

		HCamera mCamera;

		Color mColor;
		Matrix4 mTransform;
		HSceneObject mActiveSO;
		bool mPickable;

		Vector<CubeData> mSolidCubeData;
		Vector<CubeData> mWireCubeData;
		Vector<SphereData> mSolidSphereData;
		Vector<SphereData> mWireSphereData;
		Vector<LineData> mLineData;
		Vector<FrustumData> mFrustumData;
		Vector<IconData> mIconData;

		UINT32 mTotalRequiredSolidVertices;
		UINT32 mTotalRequiredSolidIndices;

		UINT32 mTotalRequiredWireVertices;
		UINT32 mTotalRequiredWireIndices;

		MeshHeapPtr mSolidMeshHeap;
		MeshHeapPtr mWireMeshHeap;
		MeshHeapPtr mIconMeshHeap;

		TransientMeshPtr mSolidMesh;
		TransientMeshPtr mWireMesh;
		TransientMeshPtr mIconMesh;

		// Immutable
		VertexDataDescPtr mSolidVertexDesc;
		VertexDataDescPtr mWireVertexDesc;
		VertexDataDescPtr mIconVertexDesc;

		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;
		IconMaterialData mIconMaterial;

		// Transient
		struct SortedIconData
		{
			float distance;
			Vector2I screenPosition;
			UINT32 iconIdx;
		};

		Vector<SortedIconData> mSortedIconData;
	};
}