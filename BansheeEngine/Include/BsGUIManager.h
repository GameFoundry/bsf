#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmInputHandler.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages the rendering of all GUI widgets in the scene. 
	 */
	class BS_EXPORT GUIManager : public CM::Module<GUIManager>
	{
	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
	private:
		std::vector<GUIWidget*> mWidgets;

		GUIWidget* mMouseOverWidget;
		GUIElement* mMouseOverElement;

		bool mLastFrameButtonState[CM::MB_Count];
		CM::Int2 mLastCursorPos;
	};
}