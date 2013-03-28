#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

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

		void renderGUI();

	private:
		void updateDirtyMeshes();
	};
}