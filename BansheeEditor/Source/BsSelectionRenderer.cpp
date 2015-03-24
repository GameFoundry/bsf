#include "BsSelectionRenderer.h"
#include "BsMesh.h"
#include "BsVertexDataDesc.h"
#include "BsCamera.h"
#include "BsCoreThread.h"
#include "BsBuiltinEditorResources.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsRenderAPI.h"
#include "BsCoreRenderer.h"
#include "BsRendererManager.h"
#include "BsSelection.h"
#include "BsSceneObject.h"
#include "BsRenderable.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	SelectionRenderer::SelectionRenderer()
		:mCore(nullptr)
	{
		HMaterial selectionMat = BuiltinEditorResources::instance().createSelectionMat();
			
		mCore = bs_new<SelectionRendererCore>(SelectionRendererCore::PrivatelyConstuct());
		gCoreAccessor().queueCommand(std::bind(&SelectionRenderer::initializeCore, this, selectionMat->getCore()));;
	}

	SelectionRenderer::~SelectionRenderer()
	{
		gCoreAccessor().queueCommand(std::bind(&SelectionRenderer::destroyCore, this, mCore));
	}

	void SelectionRenderer::initializeCore(const SPtr<MaterialCore>& initData)
	{
		mCore->initialize(initData);
	}

	void SelectionRenderer::destroyCore(SelectionRendererCore* core)
	{
		bs_delete(core);
	}

	void SelectionRenderer::update(const CameraHandlerPtr& camera)
	{
		Vector<SelectionRenderer::ObjectData> objects;

		const Vector<HSceneObject>& sceneObjects = Selection::instance().getSceneObjects();
		for (auto& so : sceneObjects)
		{
			HRenderable renderable = so->getComponent<Renderable>();
			if (renderable != nullptr && renderable->getMesh() != nullptr)
			{
				objects.push_back(ObjectData());

				ObjectData& newObjData = objects.back();
				newObjData.worldTfrm = so->getWorldTfrm();
				newObjData.mesh = renderable->getMesh()->getCore();
			}
		}

		RenderTargetPtr rt = camera->getViewport()->getTarget();
		gCoreAccessor().queueCommand(std::bind(&SelectionRendererCore::updateData, mCore, rt->getCore(), objects));
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
		fragParams->getParam("color", mColor);
		
		CoreRendererPtr activeRenderer = RendererManager::instance().getActive();
		activeRenderer->onCorePostRenderViewport.connect(std::bind(&SelectionRendererCore::render, this, _1));
	}

	void SelectionRendererCore::updateData(const SPtr<RenderTargetCore>& rt, const Vector<SelectionRenderer::ObjectData>& objects)
	{
		mSceneRenderTarget = rt;
		mObjects = objects;
	}

	void SelectionRendererCore::render(const CameraHandlerCore& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mSceneRenderTarget == nullptr)
			return;

		if (camera.getViewport()->getTarget() != mSceneRenderTarget)
			return;

		Matrix4 viewProjMat = camera.getProjectionMatrixRS() * camera.getViewMatrix();

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