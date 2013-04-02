#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderOperation.h"

namespace CamelotEngine
{
	/**
	 * @brief	Manages the rendering of all GUI widgets in the scene. 
	 */
	class CM_EXPORT GUIManager : public Module<GUIManager>
	{
	public:
		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		vector<RenderObject>::type getRenderObjects();

	private:
		vector<GUIWidget*>::type mWidgets;
		vector<MeshHandle>::type mMeshes;
		vector<MaterialHandle>::type mMaterials;

		void updateDirtyMeshes();
	};
}