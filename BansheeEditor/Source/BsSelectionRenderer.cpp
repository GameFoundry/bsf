#include "BsSelectionRenderer.h"
#include "BsMesh.h"
#include "BsVertexDataDesc.h"
#include "BsCCamera.h"
#include "BsCoreThread.h"
#include "BsBuiltinEditorResources.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsRenderAPI.h"
#include "BsCoreRenderer.h"
#include "BsRendererManager.h"
#include "BsSelection.h"
#include "BsSceneObject.h"
#include "BsCRenderable.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	SelectionRenderer::SelectionRenderer()
		:mCore(nullptr)
	{
		HMaterial selectionMat = BuiltinEditorResources::instance().createSelectionMat();
			
		mCore.store(bs_new<SelectionRendererCore>(SelectionRendererCore::PrivatelyConstuct()), std::memory_order_release);

		gCoreAccessor().queueCommand(std::bind(&SelectionRenderer::initializeCore, this, selectionMat->getCore()));;
	}

	SelectionRenderer::~SelectionRenderer()
	{
		gCoreAccessor().queueCommand(std::bind(&SelectionRenderer::destroyCore, this, mCore.load(std::memory_order_relaxed)));
	}

	void SelectionRenderer::initializeCore(const SPtr<MaterialCore>& initData)
	{
		mCore.load(std::memory_order_acquire)->initialize(initData);
	}

	void SelectionRenderer::destroyCore(SelectionRendererCore* core)
	{
		bs_delete(core);
	}

	void SelectionRenderer::update(const CameraPtr& camera)
	{
		Vector<ObjectData> objects;

		const Vector<HSceneObject>& sceneObjects = Selection::instance().getSceneObjects();
		for (auto& so : sceneObjects)
		{
			HRenderable renderable = so->getComponent<CRenderable>();
			if (renderable != nullptr && renderable->getMesh() != nullptr)
			{
				objects.push_back(ObjectData());

				ObjectData& newObjData = objects.back();
				newObjData.worldTfrm = so->getWorldTfrm();
				newObjData.mesh = renderable->getMesh()->getCore();
			}
		}

		SelectionRendererCore* core = mCore.load(std::memory_order_relaxed);
		gCoreAccessor().queueCommand(std::bind(&SelectionRendererCore::updateData, core, camera->getCore(), objects));
	}

	const Color SelectionRendererCore::SELECTION_COLOR = Color(1.0f, 1.0f, 1.0f, 0.3f);

	SelectionRendererCore::SelectionRendererCore(const PrivatelyConstuct& dummy)
	{
	}

	void SelectionRendererCore::initialize(const SPtr<MaterialCore>& mat)
	{
		THROW_IF_NOT_CORE_THREAD;

		mMaterial = mat;

		SPtr<GpuParamsCore> vertParams = mat->getPassParameters(0)->mVertParams;
		vertParams->getParam("matWorldViewProj", mMatWorldViewProj);

		SPtr<GpuParamsCore> fragParams = mat->getPassParameters(0)->mFragParams;
		fragParams->getParam("selColor", mColor);
	}

	void SelectionRendererCore::updateData(const SPtr<CameraCore>& camera, const Vector<SelectionRenderer::ObjectData>& objects)
	{
		if (mCamera != camera)
		{
			CoreRendererPtr activeRenderer = RendererManager::instance().getActive();
			if (mCamera != nullptr)
				activeRenderer->_unregisterRenderCallback(mCamera.get(), 10);

			if (camera != nullptr)
				activeRenderer->_registerRenderCallback(camera.get(), 10, std::bind(&SelectionRendererCore::render, this));
		}

		mCamera = camera;
		mObjects = objects;
	}

	void SelectionRendererCore::render()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mCamera == nullptr)
			return;

		Matrix4 viewProjMat = mCamera->getProjectionMatrixRS() * mCamera->getViewMatrix();

		for (auto& objData : mObjects)
		{
			Matrix4 worldViewProjMat = viewProjMat * objData.worldTfrm;

			mMatWorldViewProj.set(worldViewProjMat);
			mColor.set(SELECTION_COLOR);

			CoreRenderer::setPass(mMaterial, 0);
			CoreRenderer::draw(objData.mesh, objData.mesh->getProperties().getSubMesh(0));
		}
	}
}