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
		renderSystem->_setDepthBufferParams();


		const vector<CameraPtr>::type& allCameras = gSceneManager().getAllCameras();
		for(auto iter = allCameras.begin(); iter != allCameras.end(); ++iter)
		{
			render(*iter);
		}

		renderSystem->_swapAllRenderTargetBuffers(false);
	}

	void ForwardRenderer::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_setViewport(camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderSystem->clearFrameBuffer(FBT_COLOUR | FBT_DEPTH, Color::Blue);
		renderSystem->_beginFrame();

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

				material->applyPass(i); // TODO - applyPass method should be more like RenderSystem::applyPass(Material, passIdx)
				renderSystem->_render(mesh->getRenderOperation());
			}
		}

		renderSystem->_endFrame();

		// TODO - Sort renderables
		// Render them
	}

	void ForwardRenderer::setPass(PassPtr pass)
	{
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
			renderSystem->_setSeparateSceneBlending(
				pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
				pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
				pass->getSceneBlendingOperation(), 
				pass->hasSeparateSceneBlendingOperations() ? pass->getSceneBlendingOperation() : pass->getSceneBlendingOperationAlpha() );
		}
		else
		{
			if(pass->hasSeparateSceneBlendingOperations( ) )
			{
				renderSystem->_setSeparateSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
					pass->getSceneBlendingOperation(), pass->getSceneBlendingOperationAlpha() );
			}
			else
			{
				renderSystem->_setSceneBlending(
					pass->getSourceBlendFactor(), pass->getDestBlendFactor(), pass->getSceneBlendingOperation() );
			}
		}

		// Set point parameters
		renderSystem->_setPointParameters(
			pass->getPointSize(),
			false, 
			false, 
			false, 
			false, 
			pass->getPointMinSize(), 
			pass->getPointMaxSize());





		// TODO - Set texture sampler states!

		// TODO - Try to limit amount of state changes, if previous state is already the same (especially with textures)

		// TODO: Disable remaining texture units
		//renderSystem->_disableTextureUnitsFrom(pass->getNumTextureUnitStates());




		// Set up non-texture related material settings
		// Depth buffer settings
		renderSystem->_setDepthBufferFunction(pass->getDepthFunction());
		renderSystem->_setDepthBufferCheckEnabled(pass->getDepthCheckEnabled());
		renderSystem->_setDepthBufferWriteEnabled(pass->getDepthWriteEnabled());
		renderSystem->_setDepthBias(pass->getDepthBiasConstant(), pass->getDepthBiasSlopeScale());

		// Alpha-reject settings
		renderSystem->_setAlphaRejectSettings(
			pass->getAlphaRejectFunction(), pass->getAlphaRejectValue(), pass->isAlphaToCoverageEnabled());

		// Set colour write mode
		// Right now we only use on/off, not per-channel
		bool colWrite = pass->getColourWriteEnabled();
		renderSystem->_setColourBufferWriteEnabled(colWrite, colWrite, colWrite, colWrite);

		// Culling mode
		renderSystem->_setCullingMode(pass->getCullingMode());

		// Polygon mode
		renderSystem->_setPolygonMode(pass->getPolygonMode());
	}
}