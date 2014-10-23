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

		void draw(const HCamera& camera);

	private:
		friend class HandleDrawManagerCore;

		void initializeCore(const MaterialProxyPtr& wireMatProxy, const MaterialProxyPtr& solidMatProxy);
		void destroyCore(HandleDrawManagerCore* core);

		static const UINT32 SPHERE_QUALITY;
		static const UINT32 WIRE_SPHERE_QUALITY;
		static const UINT32 ARC_QUALITY;

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