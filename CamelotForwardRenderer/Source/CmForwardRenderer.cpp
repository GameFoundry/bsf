#include "CmForwardRenderer.h"
#include "CmCamera.h"
#include "CmSceneManager.h"
#include "CmDeferredRenderContext.h"
#include "CmRenderable.h"
#include "CmMaterial.h"
#include "CmMesh.h"
#include "CmPass.h"
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

		// TODO - No point in setting these each frame?
		renderContext->setInvertVertexWinding(false);
		renderContext->setDepthBufferParams();

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

		// Set scene blending
		if ( pass->hasSeparateSceneBlending( ) )
		{
			renderContext->setSeparateSceneBlending(
				pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
				pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
				pass->getSceneBlendingOperation(), 
				pass->hasSeparateSceneBlendingOperations() ? pass->getSceneBlendingOperation() : pass->getSceneBlendingOperationAlpha() );
		}
		else
		{
			if(pass->hasSeparateSceneBlendingOperations( ) )
			{
				renderContext->setSeparateSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSceneBlendingOperation(), pass->getSceneBlendingOperationAlpha() );
			}
			else
			{
				renderContext->setSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(), pass->getSceneBlendingOperation() );
			}
		}

		// Set point parameters
		renderContext->setPointParameters(
			pass->getPointSize(),
			false, 
			false, 
			false, 
			false, 
			pass->getPointMinSize(), 
			pass->getPointMaxSize());

		// TODO - Try to limit amount of state changes, if previous state is already the same (especially with textures)

		// TODO: Disable remaining texture units
		//renderSystem->_disableTextureUnitsFrom(pass->getNumTextures());


		// Set up non-texture related material settings
		// Depth buffer settings
		renderContext->setDepthBufferFunction(pass->getDepthFunction());
		renderContext->setDepthBufferCheckEnabled(pass->getDepthCheckEnabled());
		renderContext->setDepthBufferWriteEnabled(pass->getDepthWriteEnabled());
		renderContext->setDepthBias(pass->getDepthBiasConstant(), pass->getDepthBiasSlopeScale());

		// Alpha-reject settings
		renderContext->setAlphaRejectSettings(
			pass->getAlphaRejectFunction(), pass->getAlphaRejectValue(), pass->isAlphaToCoverageEnabled());

		// Set colour write mode
		// Right now we only use on/off, not per-channel
		bool colWrite = pass->getColourWriteEnabled();
		renderContext->setColorBufferWriteEnabled(colWrite, colWrite, colWrite, colWrite);

		// Culling mode
		renderContext->setCullingMode(pass->getCullingMode());

		// Polygon mode
		renderContext->setPolygonMode(pass->getPolygonMode());
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