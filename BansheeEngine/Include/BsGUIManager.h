#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIKeyEvent.h"
#include "CmModule.h"
#include "CmInput.h"
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

			CM::Vector<CM::HMesh>::type cachedMeshes;
			CM::Vector<CM::HMaterial>::type cachedMaterials;
			CM::Vector<GUIWidget*>::type cachedWidgetsPerMesh;
			CM::Vector<GUIWidget*>::type widgets;
			bool isDirty;
		};

	public:
		GUIManager();
		~GUIManager();

		void registerWidget(GUIWidget* widget);
		void unregisterWidget(GUIWidget* widget);

		void update();
		void render(CM::ViewportPtr& target, CM::CoreAccessor& coreAccessor);
	private:
		CM::Vector<GUIWidget*>::type mWidgets;
		CM::UnorderedMap<const CM::Viewport*, GUIRenderData>::type mCachedGUIData;

		// Element and widget mouse is currently over
		GUIWidget* mMouseOverWidget;
		GUIElement* mMouseOverElement;

		// Element and widget that's being clicked on
		GUIWidget* mActiveWidget;
		GUIElement* mActiveElement;
		GUIMouseButton mActiveMouseButton;

		// Element and widget that currently have the keyboard focus
		GUIWidget* mKeyboardFocusWidget;
		GUIElement* mKeyboardFocusElement;

		bool mSeparateMeshesByWidget;
		CM::Int2 mLastCursorLocalPos;

		GUIMouseEvent mMouseEvent;
		GUIKeyEvent mKeyEvent;

		boost::signals::connection mOnButtonDownConn;
		boost::signals::connection mOnButtonUpConn;
		boost::signals::connection mOnMouseMovedConn;
		boost::signals::connection mOnTextInputConn;

		boost::signals::connection mWindowGainedFocusConn;
		boost::signals::connection mWindowLostFocusConn;
		boost::signals::connection mWindowMovedOrResizedConn;

		void updateMeshes();

		void onButtonDown(const CM::ButtonEvent& event);
		void onButtonUp(const CM::ButtonEvent& event);

		void onMouseMoved(const CM::MouseEvent& event);
		void onTextInput(const CM::TextInputEvent& event);

		void onWindowFocusGained(CM::RenderWindow& win);
		void onWindowFocusLost(CM::RenderWindow& win);
		void onWindowMovedOrResized(CM::RenderWindow& win);

		GUIMouseButton buttonToMouseButton(CM::ButtonCode code) const;
		CM::Int2 getWidgetRelativePos(const GUIWidget& widget, const CM::Int2& screenPos) const;
	};
}