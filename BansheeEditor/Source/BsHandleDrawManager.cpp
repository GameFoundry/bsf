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

	HandleDrawManager::HandleDrawManager(const HCamera& camera)
		:mCamera(camera)
	{
		mSceneRenderTarget = mCamera->getViewport()->getTarget();
		mDrawHelper = bs_new<DrawHelper>();

		mSolidMaterial.material = BuiltinEditorResources::instance().createSolidGizmoMat();
		mWireMaterial.material = BuiltinEditorResources::instance().createWireGizmoMat();

		mSolidMaterial.proxy = mSolidMaterial.material->_createProxy();
		mWireMaterial.proxy = mWireMaterial.material->_createProxy();

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::initializeCore, this));
	}

	HandleDrawManager::~HandleDrawManager()
	{
		bs_delete(mDrawHelper);

		if (mSolidMesh != nullptr)
			mDrawHelper->releaseSolidMesh(mSolidMesh);

		if (mWireMesh != nullptr)
			mDrawHelper->releaseWireMesh(mWireMesh);
	}

	void HandleDrawManager::initializeCore()
	{
		THROW_IF_NOT_CORE_THREAD;

		// TODO - Make a better interface when dealing with parameters through proxies?
		{
			MaterialProxyPtr proxy = mWireMaterial.proxy;
			GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mWireMaterial.mViewProj);
		}

		{
			MaterialProxyPtr proxy = mSolidMaterial.proxy;
			GpuParamsPtr vertParams = proxy->params[proxy->passes[0].vertexProgParamsIdx];

			vertParams->getParam("matViewProj", mSolidMaterial.mViewProj);
			vertParams->getParam("matViewIT", mSolidMaterial.mViewIT);
		}

		RendererPtr activeRenderer = RendererManager::instance().getActive();
		activeRenderer->onCorePostRenderViewport.connect(std::bind(&HandleDrawManager::coreRender, this, _1));
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

	void HandleDrawManager::draw()
	{
		if (mSolidMesh != nullptr)
			mDrawHelper->releaseSolidMesh(mSolidMesh);

		if (mWireMesh != nullptr)
			mDrawHelper->releaseWireMesh(mWireMesh);

		mSolidMesh = mDrawHelper->buildSolidMesh();
		mWireMesh = mDrawHelper->buildWireMesh();

		MeshProxyPtr solidMeshProxy = mSolidMesh->_createProxy(0);
		MeshProxyPtr wireMeshProxy = mWireMesh->_createProxy(0);

		gCoreAccessor().queueCommand(std::bind(&HandleDrawManager::coreUpdateData, this, solidMeshProxy, wireMeshProxy));

		mDrawHelper->clear();
	}

	void HandleDrawManager::coreUpdateData(const MeshProxyPtr& solidMeshProxy, const MeshProxyPtr& wireMeshProxy)
	{
		mSolidMeshProxy = solidMeshProxy;
		mWireMeshProxy = wireMeshProxy;
	}

	void HandleDrawManager::coreRender(const CameraProxy& camera)
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

		coreRenderSolid(camera.viewMatrix, camera.projMatrix, mSolidMeshProxy);
		coreRenderWire(camera.viewMatrix, camera.projMatrix, mWireMeshProxy);
	}

	void HandleDrawManager::coreRenderSolid(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;
		Matrix4 viewIT = viewMatrix.inverse().transpose();

		mSolidMaterial.mViewProj.set(viewProjMat);
		mSolidMaterial.mViewIT.set(viewIT);

		Renderer::setPass(*mSolidMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}

	void HandleDrawManager::coreRenderWire(Matrix4 viewMatrix, Matrix4 projMatrix, MeshProxyPtr meshProxy)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = projMatrix * viewMatrix;
		Matrix4 viewIT = viewMatrix.inverse().transpose();

		mWireMaterial.mViewProj.set(viewProjMat);

		Renderer::setPass(*mWireMaterial.proxy, 0);
		Renderer::draw(*meshProxy);
	}
}