//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Scene/BsSelectionRenderer.h"
#include "Mesh/BsMesh.h"
#include "RenderAPI/BsVertexDataDesc.h"
#include "Components/BsCCamera.h"
#include "CoreThread/BsCoreThread.h"
#include "Utility/BsBuiltinEditorResources.h"
#include "Material/BsMaterial.h"
#include "RenderAPI/BsGpuParams.h"
#include "Material/BsGpuParamsSet.h"
#include "RenderAPI/BsRenderAPI.h"
#include "Renderer/BsRenderer.h"
#include "Renderer/BsRendererManager.h"
#include "Scene/BsSelection.h"
#include "Scene/BsSceneObject.h"
#include "Components/BsCRenderable.h"
#include "Renderer/BsRenderable.h"
#include "Scene/BsSceneManager.h"
#include "Renderer/BsRendererUtility.h"
#include "Animation/BsAnimationManager.h"
#include "Animation/BsSkeleton.h"
#include "RenderAPI/BsGpuBuffer.h"
#include "Renderer/BsRenderer.h"
#include "Renderer/BsRenderableElement.h"

using namespace std::placeholders;

namespace bs
{
	SelectionRenderer::SelectionRenderer()
	{
		HMaterial selectionMat = BuiltinEditorResources::instance().createSelectionMat();
			
		mRenderer = RendererExtension::create<ct::SelectionRendererCore>(selectionMat->getCore());
	}

	SelectionRenderer::~SelectionRenderer()
	{ }

	void SelectionRenderer::update(const SPtr<Camera>& camera)
	{
		Vector<SPtr<ct::Renderable>> objects;

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

		ct::SelectionRendererCore* renderer = mRenderer.get();
		gCoreThread().queueCommand(std::bind(&ct::SelectionRendererCore::updateData, renderer, camera->getCore(), objects));
	}

	namespace ct
	{
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

		SPtr<Material> mat = any_cast<SPtr<Material>>(data);
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

			SPtr<GpuParams> params = mParams[i]->getGpuParams();
			params->getParam(GPT_VERTEX_PROGRAM, "matWorldViewProj", mMatWorldViewProj[i]);

			RenderableAnimType animType = (RenderableAnimType)i;
			if(animType == RenderableAnimType::Skinned || animType == RenderableAnimType::SkinnedMorph)
				params->getBufferParam(GPT_VERTEX_PROGRAM, "boneMatrices", mBoneMatrices[i]);

			params->getParam(GPT_FRAGMENT_PROGRAM, "selColor", mColor[i]);
		}
	}

	void SelectionRendererCore::updateData(const SPtr<Camera>& camera, const Vector<SPtr<Renderable>>& objects)
	{
		mCamera = camera;
		mObjects = objects;
	}

	bool SelectionRendererCore::check(const Camera& camera)
	{
		return mCamera.get() == &camera;
	}

	void SelectionRendererCore::render(const Camera& camera)
	{
		THROW_IF_NOT_CORE_THREAD;

		Matrix4 viewProjMat = mCamera->getProjectionMatrixRS() * mCamera->getViewMatrix();

		SPtr<Renderer> renderer = gRenderer();

		for (auto& renderable : mObjects)
		{
			SPtr<Mesh> mesh = renderable->getMesh();
			if (mesh == nullptr)
				continue;

			SPtr<GpuBuffer> boneMatrixBuffer = renderable->getBoneMatrixBuffer();
			SPtr<VertexBuffer> morphShapeBuffer = renderable->getMorphShapeBuffer();
			SPtr<VertexDeclaration> morphVertexDeclaration = renderable->getMorphVertexDeclaration();

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
}