#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	class HandleDrawManagerCore;

	class BS_ED_EXPORT HandleDrawManager
	{
	public:
		HandleDrawManager();
		~HandleDrawManager();

		void setColor(const Color& color);
		void setTransform(const Matrix4& transform);

		void drawCube(const Vector3& position, const Vector3& extents, float size = 1.0f);
		void drawSphere(const Vector3& position, float radius, float size = 1.0f);
		void drawWireCube(const Vector3& position, const Vector3& extents, float size = 1.0f);
		void drawWireSphere(const Vector3& position, float radius, float size = 1.0f);
		void drawCone(const Vector3& base, const Vector3& normal, float height, float radius, float size = 1.0f);
		void drawLine(const Vector3& start, const Vector3& end, float size = 1.0f);
		void drawDisc(const Vector3& position, const Vector3& normal, float radius, float size = 1.0f);
		void drawWireDisc(const Vector3& position, const Vector3& normal, float radius, float size = 1.0f);
		void drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f);
		void drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f);
		void drawRect(const Rect3& area, float size = 1.0f);

		void draw(const CameraHandlerPtr& camera);

	private:
		friend class HandleDrawManagerCore;

		void initializeCore(const MaterialProxyPtr& wireMatProxy, const MaterialProxyPtr& solidMatProxy);
		void destroyCore(HandleDrawManagerCore* core);

		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const UINT32 ARC_QUALITY;

		Matrix4 mTransform;
		HandleDrawManagerCore* mCore;
		DrawHelper* mDrawHelper;
	};

	class BS_ED_EXPORT HandleDrawManagerCore
	{
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

		enum class MeshType
		{
			Solid, Wire
		};

		struct MeshProxyData
		{
			MeshProxyData(const MeshProxyPtr& proxy, MeshType type)
				:proxy(proxy), type(type)
			{ }

			MeshProxyPtr proxy;
			MeshType type;
		};

		struct PrivatelyConstruct { };

	public:
		HandleDrawManagerCore(const PrivatelyConstruct& dummy) { }

	private:
		friend class HandleDrawManager;

		void initialize(const MaterialProxyPtr& wireMatProxy, const MaterialProxyPtr& solidMatProxy);

		void updateData(const RenderTargetPtr& rt, const Vector<MeshProxyData>& proxies);
		void render(const CameraProxy& camera);

		RenderTargetPtr mSceneRenderTarget;
		Vector<MeshProxyData> mProxies;

		// Immutable
		SolidMaterialData mSolidMaterial;
		WireMaterialData mWireMaterial;
	};
}