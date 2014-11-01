#include "BsHandleDrawManager.h"
#include "BsDrawHelper.h"
#include "BsMaterial.h"
#include "BsBuiltinEditorResources.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsTransientMesh.h"
#include "BsCameraHandler.h"
#include "BsSceneObject.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 HandleDrawManager::SPHERE_QUALITY = 1;
	const UINT32 HandleDrawManager::WIRE_SPHERE_QUALITY = 10;
	const UINT32 HandleDrawManager::ARC_QUALITY = 10;

	HandleDrawManager::HandleDrawManager()
		:mCore(nullptr)
	{
		mTransform = Matrix4::IDENTITY;
		mDrawHelper = bs_new<DrawHelper>();

		HMaterial solidMaterial = BuiltinEditorResources::instance().createSolidHandleMat();
		HMaterial wireMaterial = BuiltinEditorResources::instance().createWireHandleMat();

		MaterialProxyPtr solidMaterialProxy = solidMaterial->_createProxy();
		MaterialProxyPtr wireMaterialProxy = wireMaterial->_createProxy();

		mCore = bs_new<HandleDrawManagerCore>(HandleDrawManagerCore::PrivatelyConstruct());

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::initializeCore, this, wireMaterialProxy, solidMaterialProxy));
	}

	HandleDrawManager::~HandleDrawManager()
	{
		bs_delete(mDrawHelper);

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::destroyCore, this, mCore));
	}

	void HandleDrawManager::initializeCore(const MaterialProxyPtr& wireMatProxy, const MaterialProxyPtr& solidMatProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		mCore->initialize(wireMatProxy, solidMatProxy);
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

	void HandleDrawManager::drawCube(const Vector3& position, const Vector3& extents, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->cube(position, extents);
	}

	void HandleDrawManager::drawSphere(const Vector3& position, float radius, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->sphere(position, radius);
	}

	void HandleDrawManager::drawWireCube(const Vector3& position, const Vector3& extents, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->wireCube(position, extents);
	}

	void HandleDrawManager::drawWireSphere(const Vector3& position, float radius, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->wireSphere(position, radius);
	}

	void HandleDrawManager::drawCone(const Vector3& base, const Vector3& normal, float height, float radius, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->cone(base, normal, height, radius);
	}

	void HandleDrawManager::drawLine(const Vector3& start, const Vector3& end, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->line(start, end);
	}

	void HandleDrawManager::drawDisc(const Vector3& position, const Vector3& normal, float radius, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->disc(position, normal, radius);
	}

	void HandleDrawManager::drawWireDisc(const Vector3& position, const Vector3& normal, float radius, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->wireDisc(position, normal, radius);
	}

	void HandleDrawManager::drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->arc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->wireArc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawRect(const Rect3& area, float size)
	{
		Matrix4 scale(Vector3::ZERO, Quaternion::IDENTITY, Vector3(size, size, size));
		mDrawHelper->setTransform(scale * mTransform);

		mDrawHelper->rectangle(area);
	}

	void HandleDrawManager::draw(const CameraHandlerPtr& camera)
	{
		mDrawHelper->clearMeshes();
		mDrawHelper->buildMeshes(DrawHelper::SortType::BackToFront, camera->getPosition());

		const Vector<DrawHelper::ShapeMeshData>& meshes = mDrawHelper->getMeshes();
		
		Vector<HandleDrawManagerCore::MeshProxyData> proxyData;
		for (auto& meshData : meshes)
		{
			if (meshData.type == DrawHelper::MeshType::Solid)
			{
				MeshProxyPtr proxy = meshData.mesh->_createProxy(0);
				proxyData.push_back(HandleDrawManagerCore::MeshProxyData(proxy, HandleDrawManagerCore::MeshType::Solid));
			}
			else // Wire
			{
				MeshProxyPtr proxy = meshData.mesh->_createProxy(0);
				proxyData.push_back(HandleDrawManagerCore::MeshProxyData(proxy, HandleDrawManagerCore::MeshType::Wire));
			}
		}

		RenderTargetPtr sceneRenderTarget = camera->getViewport()->getTarget();

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManagerCore::updateData, mCore, 
			sceneRenderTarget, proxyData));

		mDrawHelper->clear();
	}

	void HandleDrawManagerCore::initialize(const MaterialProxyPtr& wireMatProxy, const MaterialProxyPtr& solidMatProxy)
	{
		// TODO - Make a better interface when dealing with parameters through proxies?
		{
			mWireMaterial.proxy = wireMatProxy;
			GpuParamsPtr vertParams = wireMatProxy->params[wireMatProxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mWireMaterial.mViewProj);
		}

		{
			mSolidMaterial.proxy = solidMatProxy;
			GpuParamsPtr vertParams = solidMatProxy->params[solidMatProxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mSolidMaterial.mViewProj);
		}

		RendererPtr activeRenderer = RendererManager::instance().getActive();
		activeRenderer->onCorePostRenderViewport.connect(std::bind(&HandleDrawManagerCore::render, this, _1));
	}

	void HandleDrawManagerCore::updateData(const RenderTargetPtr& rt, const Vector<MeshProxyData>& proxies)
	{
		mSceneRenderTarget = rt;
		mProxies = proxies;
	}

	void HandleDrawManagerCore::render(const CameraProxy& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (camera.viewport.getTarget() != mSceneRenderTarget)
			return;

		float width = (float)mSceneRenderTarget->getCore()->getProperties().getWidth();
		float height = (float)mSceneRenderTarget->getCore()->getProperties().getHeight();

		Rect2 normArea = camera.viewport.getNormArea();

		Rect2I screenArea;
		screenArea.x = (int)(normArea.x * width);
		screenArea.y = (int)(normArea.y * height);
		screenArea.width = (int)(normArea.width * width);
		screenArea.height = (int)(normArea.height * height);

		Matrix4 viewProjMat = camera.projMatrix * camera.viewMatrix;
		mSolidMaterial.mViewProj.set(viewProjMat);
		mWireMaterial.mViewProj.set(viewProjMat);

		MeshType currentType = MeshType::Solid;
		if (mProxies.size() > 0)
		{
			currentType = mProxies[0].type;

			if (currentType == MeshType::Solid)
				Renderer::setPass(*mSolidMaterial.proxy, 0);
			else
				Renderer::setPass(*mWireMaterial.proxy, 0);
		}

		for (auto& proxyData : mProxies)
		{
			if (currentType != proxyData.type)
			{
				if (proxyData.type == MeshType::Solid)
					Renderer::setPass(*mSolidMaterial.proxy, 0);
				else
					Renderer::setPass(*mWireMaterial.proxy, 0);

				currentType = proxyData.type;
			}

			Renderer::draw(*proxyData.proxy);
		}
	}
}