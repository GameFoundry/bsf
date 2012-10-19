#include "CmForwardRenderManager.h"
#include "CmCamera.h"
#include "CmSceneManager.h"

namespace CamelotEngine
{
	void ForwardRenderManager::renderAll() 
	{
		const vector<CameraPtr>::type& allCameras = gSceneManager().getAllCameras();

		for(auto iter = allCameras.begin(); iter != allCameras.end(); ++iter)
		{
			render(*iter);
		}
	}

	void ForwardRenderManager::render(const CameraPtr camera) 
	{
		vector<RenderablePtr>::type allRenderables = gSceneManager().getVisibleRenderables(camera);

		// TODO - Sort renderables
		// Render them
	}

}