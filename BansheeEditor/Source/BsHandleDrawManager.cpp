#include "BsHandleDrawManager.h"
#include "BsDrawHelper.h"
#include "BsMaterial.h"
#include "BsBuiltinEditorResources.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsCoreRenderer.h"
#include "BsTransientMesh.h"
#include "BsCamera.h"
#include "BsRendererUtility.h"
#include "BsSceneObject.h"
#include "BsTime.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 HandleDrawManager::SPHERE_QUALITY = 1;
	const UINT32 HandleDrawManager::WIRE_SPHERE_QUALITY = 10;
	const UINT32 HandleDrawManager::ARC_QUALITY = 10;

	HandleDrawManager::HandleDrawManager()
		:mCore(nullptr), mLastFrameIdx((UINT64)-1)
	{
		mTransform = Matrix4::IDENTITY;
		mDrawHelper = bs_new<DrawHelper>();

		HMaterial solidMaterial = BuiltinEditorResources::instance().createSolidHandleMat();
		HMaterial wireMaterial = BuiltinEditorResources::instance().createWireHandleMat();

		SPtr<MaterialCore> solidMaterialProxy = solidMaterial->getCore();
		SPtr<MaterialCore> wireMaterialProxy = wireMaterial->getCore();

		mCore.store(bs_new<HandleDrawManagerCore>(HandleDrawManagerCore::PrivatelyConstruct()), std::memory_order_release);

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::initializeCore, this, wireMaterialProxy, solidMaterialProxy));
	}

	HandleDrawManager::~HandleDrawManager()
	{
		clearMeshes();
		bs_delete(mDrawHelper);

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::destroyCore, this, mCore.load(std::memory_order_relaxed)));
	}

	void HandleDrawManager::initializeCore(const SPtr<MaterialCore>& wireMat, const SPtr<MaterialCore>& solidMat)
	{
		THROW_IF_NOT_CORE_THREAD;

		mCore.load(std::memory_order_acquire)->initialize(wireMat, solidMat);
	}

	void HandleDrawManager::destroyCore(HandleDrawManagerCore* core)
	{
		THROW_IF_NOT_CORE_THREAD;

		bs_delete(core);
	}

	void HandleDrawManager::setColor(const Color& color)
	{
		mDrawHelper->setColor(color);
	}

	void HandleDrawManager::setTransform(const Matrix4& transform)
	{
		mTransform = transform;
	}

	void HandleDrawManager::setLayer(UINT64 layer)
	{
		mDrawHelper->setLayer(layer);
	}

	void HandleDrawManager::drawCube(const Vector3& position, const Vector3& extents, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->cube(position, extents);
	}

	void HandleDrawManager::drawSphere(const Vector3& position, float radius, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->sphere(position, radius);
	}

	void HandleDrawManager::drawWireCube(const Vector3& position, const Vector3& extents, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->wireCube(position, extents);
	}

	void HandleDrawManager::drawWireSphere(const Vector3& position, float radius, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->wireSphere(position, radius);
	}

	void HandleDrawManager::drawCone(const Vector3& base, const Vector3& normal, float height, float radius, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);

		mDrawHelper->setTransform(mTransform * scale);
		mDrawHelper->cone(base, normal, height, radius);
	}

	void HandleDrawManager::drawLine(const Vector3& start, const Vector3& end, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->line(start, end);
	}

	void HandleDrawManager::drawDisc(const Vector3& position, const Vector3& normal, float radius, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->disc(position, normal, radius);
	}

	void HandleDrawManager::drawWireDisc(const Vector3& position, const Vector3& normal, float radius, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->wireDisc(position, normal, radius);
	}

	void HandleDrawManager::drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->arc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->wireArc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawRect(const Rect3& area, float size)
	{
		Matrix4 scale = Matrix4::scaling(size);
		mDrawHelper->setTransform(mTransform * scale);

		mDrawHelper->rectangle(area);
	}

	void HandleDrawManager::draw(const CameraPtr& camera)
	{
		HandleDrawManagerCore* core = mCore.load(std::memory_order_relaxed);

		// Clear meshes from previous frame
		UINT64 frameIdx = gTime().getFrameIdx();
		if(frameIdx != mLastFrameIdx)
		{
			gCoreAccessor().queueCommand(std::bind(&HandleDrawManagerCore::clearQueued, core));

			clearMeshes();
			mLastFrameIdx = frameIdx;
		}

		mDrawHelper->buildMeshes(DrawHelper::SortType::BackToFront, camera->getPosition(), camera->getLayers());

		const Vector<DrawHelper::ShapeMeshData>& meshes = mDrawHelper->getMeshes();
		mActiveMeshes.push_back(meshes);

		Vector<HandleDrawManagerCore::MeshData> proxyData;
		for (auto& meshData : meshes)
		{
			if (meshData.type == DrawHelper::MeshType::Solid)
			{
				proxyData.push_back(HandleDrawManagerCore::MeshData(
					meshData.mesh->getCore(), HandleDrawManagerCore::MeshType::Solid));
			}
			else // Wire
			{
				proxyData.push_back(HandleDrawManagerCore::MeshData(
					meshData.mesh->getCore(), HandleDrawManagerCore::MeshType::Wire));
			}
		}

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManagerCore::queueForDraw, core, camera->getCore(), proxyData));
	}

	void HandleDrawManager::clear()
	{
		mDrawHelper->clear();
	}

	void HandleDrawManager::clearMeshes()
	{
		for (auto entry : mActiveMeshes)
			mDrawHelper->clearMeshes(entry);

		mActiveMeshes.clear();
	}

	HandleDrawManagerCore::~HandleDrawManagerCore()
	{
		clearQueued();
	}

	void HandleDrawManagerCore::initialize(const SPtr<MaterialCore>& wireMat, const SPtr<MaterialCore>& solidMat)
	{
		{
			mWireMaterial.mat = wireMat;
			SPtr<GpuParamsCore> vertParams = wireMat->getPassParameters(0)->mVertParams;

			vertParams->getParam("matViewProj", mWireMaterial.mViewProj);
		}

		{
			mSolidMaterial.mat = solidMat;
			SPtr<GpuParamsCore> vertParams = solidMat->getPassParameters(0)->mVertParams;
			SPtr<GpuParamsCore> fragParams = solidMat->getPassParameters(0)->mFragParams;

			vertParams->getParam("matViewProj", mSolidMaterial.mViewProj);
			fragParams->getParam("viewDir", mSolidMaterial.mViewDir);
		}
	}

	void HandleDrawManagerCore::queueForDraw(const SPtr<CameraCore>& camera, const Vector<MeshData>& meshes)
	{
		CoreRendererPtr activeRenderer = RendererManager::instance().getActive();
		if (camera != nullptr)
		{
			UINT32 idx = (UINT32)mQueuedData.size();
			mQueuedData.push_back({ camera, meshes });

			activeRenderer->_registerRenderCallback(camera.get(), 20, std::bind(&HandleDrawManagerCore::render, this, idx));
		}
	}

	void HandleDrawManagerCore::clearQueued()
	{
		CoreRendererPtr activeRenderer = RendererManager::instance().getActive();
		for (auto& entry : mQueuedData)
			activeRenderer->_unregisterRenderCallback(entry.camera.get(), 20);

		mQueuedData.clear();
	}

	void HandleDrawManagerCore::render(UINT32 queuedDataIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		const QueuedData& queueData = mQueuedData[queuedDataIdx];
		SPtr<CameraCore> camera = queueData.camera;
		const Vector<MeshData>& meshes = queueData.meshes;

		SPtr<RenderTargetCore> renderTarget = camera->getViewport()->getTarget();

		float width = (float)renderTarget->getProperties().getWidth();
		float height = (float)renderTarget->getProperties().getHeight();

		Rect2 normArea = camera->getViewport()->getNormArea();

		Rect2I screenArea;
		screenArea.x = (int)(normArea.x * width);
		screenArea.y = (int)(normArea.y * height);
		screenArea.width = (int)(normArea.width * width);
		screenArea.height = (int)(normArea.height * height);

		Matrix4 viewProjMat = camera->getProjectionMatrixRS() * camera->getViewMatrix();
		mSolidMaterial.mViewProj.set(viewProjMat);
		mSolidMaterial.mViewDir.set((Vector4)camera->getForward());
		mWireMaterial.mViewProj.set(viewProjMat);

		MeshType currentType = MeshType::Solid;
		if (meshes.size() > 0)
		{
			currentType = meshes[0].type;

			if (currentType == MeshType::Solid)
				gRendererUtility().setPass(mSolidMaterial.mat, 0);
			else
				gRendererUtility().setPass(mWireMaterial.mat, 0);
		}

		for (auto& meshData : meshes)
		{
			if (currentType != meshData.type)
			{
				if (meshData.type == MeshType::Solid)
					gRendererUtility().setPass(mSolidMaterial.mat, 0);
				else
					gRendererUtility().setPass(mWireMaterial.mat, 0);

				currentType = meshData.type;
			}

			gRendererUtility().draw(meshData.mesh, meshData.mesh->getProperties().getSubMesh(0));
		}
	}
}