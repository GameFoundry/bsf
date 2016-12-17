//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSelectionRenderer.h"
#include "BsMesh.h"
#include "BsVertexDataDesc.h"
#include "BsCCamera.h"
#include "BsCoreThread.h"
#include "BsBuiltinEditorResources.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsGpuParamsSet.h"
#include "BsRenderAPI.h"
#include "BsCoreRenderer.h"
#include "BsRendererManager.h"
#include "BsSelection.h"
#include "BsSceneObject.h"
#include "BsCRenderable.h"
#include "BsRenderable.h"
#include "BsSceneManager.h"
#include "BsRendererUtility.h"
#include "BsAnimationManager.h"
#include "BsSkeleton.h"
#include "BsGpuBuffer.h"
#include "BsRenderer.h"
#include "BsRenderableElement.h"

using namespace std::placeholders;

namespace bs
{
	SelectionRenderer::SelectionRenderer()
	{
		HMaterial selectionMat = BuiltinEditorResources::instance().createSelectionMat();
			
		mRenderer = RendererExtension::create<SelectionRendererCore>(selectionMat->getCore());
	}

	SelectionRenderer::~SelectionRenderer()
	{ }

	void SelectionRenderer::update(const SPtr<Camera>& camera)
	{
		Vector<SPtr<RenderableCore>> objects;

		const Vector<HSceneObject>& sceneObjects = Selection::instance().getSceneObjects();
		const Map<Renderable*, SceneRenderableData>& renderables = SceneManager::instance().getAllRenderables();

		for (auto& renderable : renderables)
		{
			for (auto& so : sceneObjects)
			{
				if (!so->getActive())
					continue;

				if (renderable.second.sceneObject != so)
					continue;

				if (renderable.first->getMesh().isLoaded())
					objects.push_back(renderable.first->getCore());
			}
		}

		SelectionRendererCore* renderer = mRenderer.get();
		gCoreThread().queueCommand(std::bind(&SelectionRendererCore::updateData, renderer, camera->getCore(), objects));
	}

	const Color SelectionRendererCore::SELECTION_COLOR = Color(1.0f, 1.0f, 1.0f, 0.3f);

	SelectionRendererCore::SelectionRendererCore()
		:RendererExtension(RenderLocation::PostLightPass, -10)
	{
	}

	void SelectionRendererCore::initialize(const Any& data)
	{
		THROW_IF_NOT_CORE_THREAD;
		
		constexpr int numTechniques = sizeof(mTechniqueIndices) / sizeof(mTechniqueIndices[0]);
		static_assert(numTechniques == (int)RenderableAnimType::Count, "Number of techniques doesn't match the number of possible animation types.");

		SPtr<MaterialCore> mat = any_cast<SPtr<MaterialCore>>(data);
		for(UINT32 i = 0; i < numTechniques; i++)
		{
			RenderableAnimType animType = (RenderableAnimType)i;
			switch (animType)
			{
				case RenderableAnimType::Skinned:
					mTechniqueIndices[i] = mat->findTechnique(RTag_Skinned);
					break;
				case RenderableAnimType::Morph:
					mTechniqueIndices[i] = mat->findTechnique(RTag_Morph);
					break;
				case RenderableAnimType::SkinnedMorph:
					mTechniqueIndices[i] = mat->findTechnique(RTag_SkinnedMorph);
					break;
				default:
					mTechniqueIndices[i] = mat->getDefaultTechnique();
					break;
			}
		}

		mMaterial = mat;

		for(UINT32 i = 0; i < 4 ; i++)
		{
			mParams[i] = mat->createParamsSet(i);

			SPtr<GpuParamsCore> params = mParams[i]->getGpuParams();
			params->getParam(GPT_VERTEX_PROGRAM, "matWorldViewProj", mMatWorldViewProj[i]);

			RenderableAnimType animType = (RenderableAnimType)i;
			if(animType == RenderableAnimType::Skinned || animType == RenderableAnimType::SkinnedMorph)
				params->getBufferParam(GPT_VERTEX_PROGRAM, "boneMatrices", mBoneMatrices[i]);

			params->getParam(GPT_FRAGMENT_PROGRAM, "selColor", mColor[i]);
		}
	}

	void SelectionRendererCore::updateData(const SPtr<CameraCore>& camera, const Vector<SPtr<RenderableCore>>& objects)
	{
		mCamera = camera;
		mObjects = objects;
	}

	bool SelectionRendererCore::check(const CameraCore& camera)
	{
		return mCamera.get() == &camera;
	}

	void SelectionRendererCore::render(const CameraCore& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		const RendererAnimationData& animData = AnimationManager::instance().getRendererData();
		Matrix4 viewProjMat = mCamera->getProjectionMatrixRS() * mCamera->getViewMatrix();

		SPtr<Renderer> renderer = gRenderer();

		for (auto& renderable : mObjects)
		{
			SPtr<MeshCore> mesh = renderable->getMesh();
			if (mesh == nullptr)
				continue;

			SPtr<GpuBufferCore> boneMatrixBuffer = renderable->getBoneMatrixBuffer();
			SPtr<VertexBufferCore> morphShapeBuffer = renderable->getMorphShapeBuffer();
			SPtr<VertexDeclarationCore> morphVertexDeclaration = renderable->getMorphVertexDeclaration();

			Matrix4 worldViewProjMat = viewProjMat * renderable->getTransform();
			UINT32 techniqueIdx = mTechniqueIndices[(int)renderable->getAnimType()];

			mMatWorldViewProj[techniqueIdx].set(worldViewProjMat);
			mColor[techniqueIdx].set(SELECTION_COLOR);
			mBoneMatrices[techniqueIdx].set(boneMatrixBuffer);

			gRendererUtility().setPass(mMaterial, 0, techniqueIdx);
			gRendererUtility().setPassParams(mParams[techniqueIdx], 0);

			UINT32 numSubmeshes = mesh->getProperties().getNumSubMeshes();

			for (UINT32 i = 0; i < numSubmeshes; i++)
			{
				if (morphVertexDeclaration == nullptr)
					gRendererUtility().draw(mesh, mesh->getProperties().getSubMesh(i));
				else
					gRendererUtility().drawMorph(mesh, mesh->getProperties().getSubMesh(i), morphShapeBuffer,
						morphVertexDeclaration);
			}
		}
	}
}