#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleDrawManager
	{
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

	public:
		HandleDrawManager(const HCamera& camera);
		~HandleDrawManager();

		void setColor(const Color& color);
		void setTransform(const Matrix4& transform);

		void drawCube(const Vector3& position, const Vector3& extents);
		void drawSphere(const Vector3& position, float radius);
		void drawWireCube(const Vector3& position, const Vector3& extents);
		void drawWireSphere(const Vector3& position, float radius);
		void drawCone(const Vector3& base, const Vector3& normal, float height, float radius);
		void drawLine(const Vector3& start, const Vector3& end);
		void drawDisc(const Vector3& position, const Vector3& normal, float radius);
		void drawWireDisc(const Vector3& position, const Vector3& normal, float radius);
		void drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle);
		void drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle);
		void drawRect(const Rect3& area);

		void draw();

	private:
		void coreRender(const CameraProxy& camera);
		void coreRenderSolid(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr mesh);
		void coreRenderWire(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr mesh);

		void coreUpdateData(const MeshProxyPtr& solidMeshProxy, const MeshProxyPtr& wireMeshProxy);
		void initializeCore();

		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const UINT32 ARC_QUALITY;

		HCamera mCamera;
		RenderTargetPtr mSceneRenderTarget;

		DrawHelper* mDrawHelper;

		TransientMeshPtr mSolidMesh;
		TransientMeshPtr mWireMesh;

		// Immutable
		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;

		// Core
		MeshProxyPtr mSolidMeshProxy;
		MeshProxyPtr mWireMeshProxy;
	};
}