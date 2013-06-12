#pragma once

#include "CmEditorPrerequisites.h"
#include "CmModule.h"
#include <boost/function.hpp>
#include <QtCore/QString>
#include <QtCore/QPoint>

namespace CamelotEditor
{
	class EditorWindowManager : public Module<EditorWindowManager>
	{		
	public:
		EditorWindowManager();

		void registerWidgetFactory(EditorWidgetFactory* factory);

		/**
		 * @brief	Opens a widget with the specified name and adds it to a window. Only one widget of a
		 * 			certain type may be open at one time.
		 *
		 * @param	name  	The name of the widget type. This will be used to find the widget factory.
		 * @param	parent	(optional) Parent to which to attach the widget to. If null, a new window will be created.
		 *
		 * @return	Returns the window the widget was added to.
		 */
		void openWidget(const QString& name, QtEditorWindow* parent = nullptr);
		boost::function<void()> getOpenCallback(const QString& name);

		QtEditorWindow* openWindow(INT32 forcedId = -1);

		QtEditorWindow* getOpenWindow(INT32 id) const;
		QtEditorWindow* getWindowAtPosition(const QPoint& globalPos, Vector<UINT32>::type windowsToIgnore = Vector<UINT32>::type()) const;

		void restoreWindowsFromPrefs();
		void saveWindowsToPrefs();

		Vector<QString>::type getAvailableWindowTypes() const;

	private:
		Map<QString, EditorWidgetFactory*>::type mFactories;
		Map<INT32, QtEditorWindow*>::type mOpenWindows;
		Map<QString, QtEditorWidget*>::type mOpenWidgets;
		UINT32 mMaxOpenWindowId;

		EditorWidgetFactory* getFactory(const QString& name) const;

		void widgetClosed(QtEditorWidget* window);
		void windowClosed(QtEditorWindow* window);
	};

	EditorWindowManager& gEditorWindowManager();
}