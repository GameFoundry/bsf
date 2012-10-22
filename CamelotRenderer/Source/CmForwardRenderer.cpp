#include "CmForwardRenderer.h"
#include "CmCamera.h"
#include "CmSceneManager.h"

namespace CamelotEngine
{
	const String& ForwardRenderer::getName() const
	{
		static String name = "ForwardRenderer";
		return name;
	}

	void ForwardRenderer::renderAll() 
	{
		const vector<CameraPtr>::type& allCameras = gSceneManager().getAllCameras();

		for(auto iter = allCameras.begin(); iter != allCameras.end(); ++iter)
		{
			render(*iter);
		}
	}

	void ForwardRenderer::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		// TODO - Sort renderables
		// Render them
	}

}