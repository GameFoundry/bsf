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
#include "CmViewport.h"
#include "CmRenderTarget.h"
#include "CmOverlayManager.h"

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

			ViewportPtr vp = (*iter)->getViewport();
			if(vp != nullptr)
			{
				RenderTargetPtr rt = vp->getTarget();

				if(rt != nullptr)
					rt->swapBuffers(); // TODO - This is wrong as potentially multiple viewports can share a single render target, and swap shouldn't
				// be done for every one of them
			}
		}
	}

	void ForwardRenderer::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		DeferredRenderContextPtr renderContext = gApplication().getPrimaryRenderContext();
		renderContext->setViewport(camera->getViewport());

		Matrix4 projMatrixCstm = camera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = camera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderContext->clear(camera->getViewport()->getTarget(), FBT_COLOUR | FBT_DEPTH, Color::Blue);
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
				PassPtr pass = material->getPass(i);
				pass->activate(renderContext);

				PassParametersPtr paramsPtr = material->getPassParameters(i);
				pass->bindParameters(renderContext, paramsPtr);

				renderContext->render(mesh->getRenderOperation());
			}
		}

		// Render overlays for this camera
		OverlayManager::instance().render(camera, renderContext);

		renderContext->endFrame();

		// TODO - Sort renderables
		// Render them
	}
}