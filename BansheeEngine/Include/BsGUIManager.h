#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"

#include "boost/signals/connection.hpp"

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

		/**
		 * @brief	Attaches a widget to a window. One widget can be attached to multiple input. Calling this
		 * 			guarantees that the widget will receieve any input received by the window.
		 */
		void attachWidgetToWindow(const CM::RenderWindow* window, GUIWidget* widget);

		void update();
	private:
		boost::signals::connection windowCreateConn;
		boost::signals::connection windowDestroyConn;

		std::vector<GUIWidget*> mWidgets;
		std::unordered_map<const CM::RenderWindow*, std::vector<GUIWidget*>> mInputMap;

		void doOnWindowCreated(CM::RenderWindow* window);
		void doOnWindowDestroyed(CM::RenderWindow* window);
	};
}