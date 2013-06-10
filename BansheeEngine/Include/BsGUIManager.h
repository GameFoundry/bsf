#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIKeyEvent.h"
#include "CmModule.h"
#include "CmInputHandler.h"
#include "CmDeferredRenderContextFwd.h"
#include <boost/signals/connection.hpp>

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

		// Element and widget that currently have the keyboard focus
		GUIWidget* mKeyboardFocusWidget;
		GUIElement* mKeyboardFocusElement;

		bool mSeparateMeshesByWidget;
		CM::Int2 mLastCursorLocalPos;

		GUIMouseEvent mMouseEvent;
		GUIKeyEvent mKeyEvent;

		boost::signals::connection mOnMouseMovedConn;
		boost::signals::connection mOnMouseDownConn;
		boost::signals::connection mOnMouseUpConn;
		boost::signals::connection mOnKeyDownConn;
		boost::signals::connection mOnKeyUpConn;

		void updateMeshes();
		void updateInput();

		void onKeyDown(CM::KeyCode keyCode);
		void onKeyUp(CM::KeyCode keyCode);

		void onMouseMoved(const CM::MouseEvent& event);
		void onMouseDown(const CM::MouseEvent& event, CM::MouseButton buttonID);
		void onMouseUp(const CM::MouseEvent& event, CM::MouseButton buttonID);

		CM::Int2 getWidgetRelativeCursorPos(const GUIWidget& widget);
	};
}