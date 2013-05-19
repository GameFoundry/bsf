#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmInputHandler.h"
#include "CmDeferredRenderContextFwd.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages the rendering and input of all GUI widgets in the scene. 
	 */
	class BS_EXPORT GUIManager : public CM::Module<GUIManager>
	{
		struct GUIRenderData
		{
			GUIRenderData()
				:isDirty(true)
			{ }

			std::vector<CM::HMesh> cachedMeshes;
			std::vector<CM::HMaterial> cachedMaterials;
			std::vector<GUIWidget*> cachedWidgetsPerMesh;
			std::vector<GUIWidget*> widgets;
			bool isDirty;
		};

	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
		void render(CM::ViewportPtr& target, CM::RenderContext& renderContext);
	private:
		std::vector<GUIWidget*> mWidgets;
		std::unordered_map<const CM::Viewport*, GUIRenderData> mCachedGUIData;

		GUIWidget* mMouseOverWidget;
		GUIElement* mMouseOverElement;

		bool mSeparateMeshesByWidget;
		bool mLastFrameButtonState[CM::MB_Count];
		CM::Int2 mLastCursorPos;

		void updateMeshes();
		void updateInput();
	};
}