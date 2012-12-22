#include "CmForwardRenderer.h"
#include "CmCamera.h"
#include "CmSceneManager.h"
#include "CmDeferredRenderContext.h"
#include "CmRenderable.h"
#include "CmMaterial.h"
#include "CmMesh.h"
#include "CmPass.h"
#include "CmBlendState.h"
#include "CmRasterizerState.h"
#include "CmDepthStencilState.h"
#include "CmApplication.h"

namespace CamelotEngine
{
	ForwardRenderer::ForwardRenderer()
	{
	}

	ForwardRenderer::~ForwardRenderer()
	{ }

	const String& ForwardRenderer::getName() const
	{
		static String name = "ForwardRenderer";
		return name;
	}

	void ForwardRenderer::renderAll() 
	{
		DeferredRenderContextPtr renderContext = gApplication().getPrimaryRenderContext();

		const vector<CameraPtr>::type& allCameras = gSceneManager().getAllCameras();
		for(auto iter = allCameras.begin(); iter != allCameras.end(); ++iter)
		{
			render(*iter);
		}

		renderContext->swapAllRenderTargetBuffers(false);
	}

	void ForwardRenderer::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		DeferredRenderContextPtr renderContext = gApplication().getPrimaryRenderContext();
		renderContext->setViewport(*camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderContext->clearFrameBuffer(FBT_COLOUR | FBT_DEPTH, Color::Blue);
		renderContext->beginFrame();

		// TODO - sort renderables by material/pass/parameters to minimize state changes
		for(auto iter = allRenderables.begin(); iter != allRenderables.end(); ++iter)
		{
			MaterialHandle material = (*iter)->getMaterial();

			if(material == nullptr || !material.isLoaded())
				continue;

			MeshHandle mesh = (*iter)->getMesh();

			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			// TODO - Renderer should ensure shader is compatible with it, and it contains all the needed parameters
			// (probably at an earlier stage). e.g. I want the user to be warned if the shader doesn't contain matViewProjection param
			// (or should we just ignore such missing parameters?)
			material->setMat4("matViewProjection", viewProjMatrix);

			for(UINT32 i = 0; i < material->getNumPasses(); i++)
			{
				setPass(material->getPass(i));
				setPassParameters(material->getPassParameters(i));

				renderContext->render(mesh->getRenderOperation());
			}
		}

		renderContext->endFrame();

		// TODO - Sort renderables
		// Render them
	}

	void ForwardRenderer::setPass(PassPtr pass)
	{
		// TODO - When applying passes, don't re-apply states that are already the same as from previous pass.
		// Also check if it's maybe the exactly the same pass as the previous one.

		mActivePass = pass;

		DeferredRenderContextPtr renderContext = gApplication().getPrimaryRenderContext();

		GpuProgramHandle vertProgram = pass->getVertexProgram();
		if(vertProgram)
		{
			renderContext->bindGpuProgram(vertProgram);
		}
		else
		{
			//if(renderSystem->isGpuProgramBound(GPT_VERTEX_PROGRAM))
				renderContext->unbindGpuProgram(GPT_VERTEX_PROGRAM);
		}

		GpuProgramHandle fragProgram = pass->getFragmentProgram();
		if(fragProgram)
		{
			renderContext->bindGpuProgram(fragProgram);
		}
		else
		{
			//if(renderSystem->isGpuProgramBound(GPT_FRAGMENT_PROGRAM))
				renderContext->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
		}

		GpuProgramHandle geomProgram = pass->getGeometryProgram();
		if(geomProgram)
		{
			renderContext->bindGpuProgram(geomProgram);
		}	
		else
		{
			//if(renderSystem->isGpuProgramBound(GPT_GEOMETRY_PROGRAM))
				renderContext->unbindGpuProgram(GPT_GEOMETRY_PROGRAM);
		}

		// The rest of the settings are the same no matter whether we use programs or not
		BlendStatePtr blendState = pass->getBlendState();
		if(blendState != nullptr)
			renderContext->setBlendState(*blendState);
		else
			renderContext->setBlendState(BlendState::getDefault());
		
		// TODO - Try to limit amount of state changes, if previous state is already the same (especially with textures)

		// TODO: Disable remaining texture units
		//renderSystem->_disableTextureUnitsFrom(pass->getNumTextures());


		// Set up non-texture related material settings
		// Stencil & depth buffer settings
		DepthStencilStatePtr depthStancilState = pass->getDepthStencilState();
		if(depthStancilState != nullptr)
			renderContext->setDepthStencilState(*depthStancilState);
		else
			renderContext->setDepthStencilState(DepthStencilState::getDefault());

		RasterizerStatePtr rasterizerState = pass->getRasterizerState();
		if(rasterizerState != nullptr)
			renderContext->setRasterizerState(*rasterizerState);
		else
			renderContext->setRasterizerState(RasterizerState::getDefault());
	}

	void ForwardRenderer::setPassParameters(PassParametersPtr params)
	{
		// TODO - When applying passes, don't re-apply states that are already the same as from previous pass.
		DeferredRenderContextPtr renderContext = gApplication().getPrimaryRenderContext();

		if(mActivePass == nullptr)
			CM_EXCEPT(InternalErrorException, "Trying to set pass parameters, but no pass is set.");

		GpuProgramHandle vertProgram = mActivePass->getVertexProgram();
		if(vertProgram)
			renderContext->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, params->mVertParams, GPV_ALL);

		GpuProgramHandle fragProgram = mActivePass->getFragmentProgram();
		if(fragProgram)
			renderContext->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, params->mFragParams, GPV_ALL);

		GpuProgramHandle geomProgram = mActivePass->getGeometryProgram();
		if(geomProgram)
			renderContext->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM, params->mGeomParams, GPV_ALL);
	}
}