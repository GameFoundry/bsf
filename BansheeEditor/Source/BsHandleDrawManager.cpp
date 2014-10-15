#include "BsHandleDrawManager.h"
#include "BsDrawHelper.h"
#include "BsMaterial.h"
#include "BsBuiltinEditorResources.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsTransientMesh.h"
#include "BsCamera.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 HandleDrawManager::SPHERE_QUALITY = 1;
	const UINT32 HandleDrawManager::WIRE_SPHERE_QUALITY = 10;
	const UINT32 HandleDrawManager::ARC_QUALITY = 10;

	HandleDrawManager::HandleDrawManager()
		:mCore(nullptr)
	{
		mDrawHelper = bs_new<DrawHelper>();

		HMaterial solidMaterial = BuiltinEditorResources::instance().createSolidGizmoMat();
		HMaterial wireMaterial = BuiltinEditorResources::instance().createWireGizmoMat();

		MaterialProxyPtr solidMaterialProxy = solidMaterial->_createProxy();
		MaterialProxyPtr wireMaterialProxy = wireMaterial->_createProxy();

		mCore = bs_new<HandleDrawManagerCore>(HandleDrawManagerCore::PrivatelyConstruct());

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::initializeCore, this, solidMaterialProxy, wireMaterialProxy));
	}

	HandleDrawManager::~HandleDrawManager()
	{
		if (mSolidMesh != nullptr)
			mDrawHelper->releaseSolidMesh(mSolidMesh);

		if (mWireMesh != nullptr)
			mDrawHelper->releaseWireMesh(mWireMesh);

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
		mDrawHelper->setTransform(transform);
	}

	void HandleDrawManager::drawCube(const Vector3& position, const Vector3& extents)
	{
		mDrawHelper->cube(position, extents);
	}

	void HandleDrawManager::drawSphere(const Vector3& position, float radius)
	{
		mDrawHelper->sphere(position, radius);
	}

	void HandleDrawManager::drawWireCube(const Vector3& position, const Vector3& extents)
	{
		mDrawHelper->wireCube(position, extents);
	}

	void HandleDrawManager::drawWireSphere(const Vector3& position, float radius)
	{
		mDrawHelper->wireSphere(position, radius);
	}

	void HandleDrawManager::drawCone(const Vector3& base, const Vector3& normal, float height, float radius)
	{
		mDrawHelper->cone(base, normal, height, radius);
	}

	void HandleDrawManager::drawLine(const Vector3& start, const Vector3& end)
	{
		mDrawHelper->line(start, end);
	}

	void HandleDrawManager::drawDisc(const Vector3& position, const Vector3& normal, float radius)
	{
		mDrawHelper->disc(position, normal, radius);
	}

	void HandleDrawManager::drawWireDisc(const Vector3& position, const Vector3& normal, float radius)
	{
		mDrawHelper->wireDisc(position, normal, radius);
	}

	void HandleDrawManager::drawArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle)
	{
		mDrawHelper->arc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawWireArc(const Vector3& position, const Vector3& normal, float radius, Degree startAngle, Degree amountAngle)
	{
		mDrawHelper->wireArc(position, normal, radius, startAngle, amountAngle);
	}

	void HandleDrawManager::drawRect(const Rect3& area)
	{
		mDrawHelper->rectangle(area);
	}

	void HandleDrawManager::draw(const HCamera& camera)
	{
		if (mSolidMesh != nullptr)
			mDrawHelper->releaseSolidMesh(mSolidMesh);

		if (mWireMesh != nullptr)
			mDrawHelper->releaseWireMesh(mWireMesh);

		mSolidMesh = mDrawHelper->buildSolidMesh();
		mWireMesh = mDrawHelper->buildWireMesh();

		MeshProxyPtr solidMeshProxy = mSolidMesh->_createProxy(0);
		MeshProxyPtr wireMeshProxy = mWireMesh->_createProxy(0);

		RenderTargetPtr sceneRenderTarget = camera->getViewport()->getTarget();

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManagerCore::updateData, mCore, 
			sceneRenderTarget, solidMeshProxy, wireMeshProxy));

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

	void HandleDrawManagerCore::updateData(const RenderTargetPtr& rt, const MeshProxyPtr& solidMeshProxy, const MeshProxyPtr& wireMeshProxy)
	{
		mSceneRenderTarget = rt;
		mSolidMeshProxy = solidMeshProxy;
		mWireMeshProxy = wireMeshProxy;
	}

	void HandleDrawManagerCore::render(const CameraProxy& camera)
	{
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

		renderSolid(camera.viewMatrix, camera.projMatrix, mSolidMeshProxy);
		renderWire(camera.viewMatrix, camera.projMatrix, mWireMeshProxy);
	}

	void HandleDrawManagerCore::renderSolid(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;

		mSolidMaterial.mViewProj.set(viewProjMat);

		Renderer::setPass(*mSolidMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}

	void HandleDrawManagerCore::renderWire(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;

		mWireMaterial.mViewProj.set(viewProjMat);

		Renderer::setPass(*mWireMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}
}