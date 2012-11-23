#include "CmForwardRenderer.h"
#include "CmCamera.h"
#include "CmSceneManager.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmRenderable.h"
#include "CmMaterial.h"
#include "CmMesh.h"
#include "CmPass.h"

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
		RenderSystem* renderSystem = RenderSystemManager::getActive();

		// TODO - No point in setting these each frame?
		renderSystem->setInvertVertexWinding(false);
		renderSystem->setDepthBufferParams();


		const vector<CameraPtr>::type& allCameras = gSceneManager().getAllCameras();
		for(auto iter = allCameras.begin(); iter != allCameras.end(); ++iter)
		{
			render(*iter);
		}

		renderSystem->swapAllRenderTargetBuffers(false);
	}

	void ForwardRenderer::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->setViewport(camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderSystem->clearFrameBuffer(FBT_COLOUR | FBT_DEPTH, Color::Blue);
		renderSystem->beginFrame();

		// TODO - sort renderables by material/pass/parameters to minimize state changes
		for(auto iter = allRenderables.begin(); iter != allRenderables.end(); ++iter)
		{
			MaterialRef material = (*iter)->getMaterial();

			if(material == nullptr || !material.isResolved())
				continue;

			MeshRef mesh = (*iter)->getMesh();

			if(mesh == nullptr || !mesh.isResolved())
				continue;

			// TODO - Renderer should ensure shader is compatible with it, and it contains all the needed parameters
			// (probably at an earlier stage). e.g. I want the user to be warned if the shader doesn't contain matViewProjection param
			// (or should we just ignore such missing parameters?)
			material->setMat4("matViewProjection", viewProjMatrix);

			for(int i = 0; i < material->getNumPasses(); i++)
			{
				setPass(material->getPass(i));
				setPassParameters(material->getPassParameters(i));

				renderSystem->render(mesh->getRenderOperation());
			}
		}

		renderSystem->endFrame();

		// TODO - Sort renderables
		// Render them
	}

	void ForwardRenderer::setPass(PassPtr pass)
	{
		// TODO - When applying passes, don't re-apply states that are already the same as from previous pass.
		// Also check if it's maybe the exactly the same pass as the previous one.

		mActivePass = pass;

		RenderSystem* renderSystem = RenderSystemManager::getActive();

		GpuProgramRef vertProgram = pass->getVertexProgram();
		if(vertProgram)
		{
			renderSystem->bindGpuProgram(vertProgram->_getBindingDelegate());
		}
		else
		{
			if(renderSystem->isGpuProgramBound(GPT_VERTEX_PROGRAM))
				renderSystem->unbindGpuProgram(GPT_VERTEX_PROGRAM);
		}

		GpuProgramRef fragProgram = pass->getFragmentProgram();
		if(fragProgram)
		{
			renderSystem->bindGpuProgram(fragProgram->_getBindingDelegate());
		}
		else
		{
			if(renderSystem->isGpuProgramBound(GPT_FRAGMENT_PROGRAM))
				renderSystem->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
		}

		GpuProgramRef geomProgram = pass->getGeometryProgram();
		if(geomProgram)
		{
			renderSystem->bindGpuProgram(geomProgram->_getBindingDelegate());
		}	
		else
		{
			if(renderSystem->isGpuProgramBound(GPT_GEOMETRY_PROGRAM))
				renderSystem->unbindGpuProgram(GPT_GEOMETRY_PROGRAM);
		}

		// The rest of the settings are the same no matter whether we use programs or not

		// Set scene blending
		if ( pass->hasSeparateSceneBlending( ) )
		{
			renderSystem->setSeparateSceneBlending(
				pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
				pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
				pass->getSceneBlendingOperation(), 
				pass->hasSeparateSceneBlendingOperations() ? pass->getSceneBlendingOperation() : pass->getSceneBlendingOperationAlpha() );
		}
		else
		{
			if(pass->hasSeparateSceneBlendingOperations( ) )
			{
				renderSystem->setSeparateSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSceneBlendingOperation(), pass->getSceneBlendingOperationAlpha() );
			}
			else
			{
				renderSystem->setSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(), pass->getSceneBlendingOperation() );
			}
		}

		// Set point parameters
		renderSystem->setPointParameters(
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
		renderSystem->setDepthBufferFunction(pass->getDepthFunction());
		renderSystem->setDepthBufferCheckEnabled(pass->getDepthCheckEnabled());
		renderSystem->setDepthBufferWriteEnabled(pass->getDepthWriteEnabled());
		renderSystem->setDepthBias(pass->getDepthBiasConstant(), pass->getDepthBiasSlopeScale());

		// Alpha-reject settings
		renderSystem->setAlphaRejectSettings(
			pass->getAlphaRejectFunction(), pass->getAlphaRejectValue(), pass->isAlphaToCoverageEnabled());

		// Set colour write mode
		// Right now we only use on/off, not per-channel
		bool colWrite = pass->getColourWriteEnabled();
		renderSystem->setColorBufferWriteEnabled(colWrite, colWrite, colWrite, colWrite);

		// Culling mode
		renderSystem->setCullingMode(pass->getCullingMode());

		// Polygon mode
		renderSystem->setPolygonMode(pass->getPolygonMode());
	}

	void ForwardRenderer::setPassParameters(PassParametersPtr params)
	{
		// TODO - When applying passes, don't re-apply states that are already the same as from previous pass.

		RenderSystem* renderSystem = RenderSystemManager::getActive();

		if(mActivePass == nullptr)
			CM_EXCEPT(InternalErrorException, "Trying to set pass parameters, but no pass is set.");

		GpuProgramRef vertProgram = mActivePass->getVertexProgram();
		if(vertProgram)
			renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, params->mVertParams, GPV_ALL);

		GpuProgramRef fragProgram = mActivePass->getFragmentProgram();
		if(fragProgram)
			renderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, params->mFragParams, GPV_ALL);

		GpuProgramRef geomProgram = mActivePass->getGeometryProgram();
		if(geomProgram)
			renderSystem->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM, params->mGeomParams, GPV_ALL);
	}
}