#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
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

			CM::vector<CM::HMesh>::type cachedMeshes;
			CM::vector<CM::HMaterial>::type cachedMaterials;
			CM::vector<GUIWidget*>::type cachedWidgetsPerMesh;
			CM::vector<GUIWidget*>::type widgets;
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
		CM::vector<GUIWidget*>::type mWidgets;
		CM::unordered_map<const CM::Viewport*, GUIRenderData>::type mCachedGUIData;

		// Element and widget mouse is currently over
		GUIWidget* mMouseOverWidget;
		GUIElement* mMouseOverElement;

		// Element and widget that's being clicked on
		GUIWidget* mActiveWidget;
		GUIElement* mActiveElement;
		CM::UINT32 mActiveMouseButton;

		bool mSeparateMeshesByWidget;
		bool mLastFrameButtonState[CM::MB_Count];
		CM::Int2 mLastCursorLocalPos;

		GUIMouseEvent mMouseEvent;

		void updateMeshes();
		void updateInput();

		CM::Int2 getWidgetRelativeCursorPos(const GUIWidget& widget);
	};
}