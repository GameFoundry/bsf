#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles opening and closing of EditorWidgets. Its primary purpose
	 * 			is to keep track of all types of widgets so they can be saved and restored
	 * 			upon program shutdown/startup, as well as being able to change widget layout on
	 * 			the fly.
	 */
	class BS_ED_EXPORT EditorWidgetManager : public Module<EditorWidgetManager>
	{
	public:
		EditorWidgetManager();
		~EditorWidgetManager();

		/**
		 * @brief	Registers a widget that can then be opened by calling "open". When loading
		 * 			a widget layout this name and callback will be used to attempt creating the widget.
		 *
		 * @param	name				  	Unique name for the widget.
		 * @param 	createCallback			Callback that returns a new instance of the widget.
		 */
		void registerWidget(const String& name, std::function<EditorWidgetBase*(EditorWidgetContainer&)> createCallback);

		/**
		 * @brief	Creates a widget with the given name. If widget is already created it returns the existing instance.
		 * 			Widget is opened in a new window.
		 *
		 * @param	name	The name of the widget.
		 *
		 * @return	Always returns the created widget, and throws an exception if it fails.
		 */
		EditorWidgetBase* open(const String& name);

		/**
		 * @brief	Creates a new widget an inserts it into the specified container.
		 */
		EditorWidgetBase* create(const String& name, EditorWidgetContainer& parentContainer);

		/**
		 * @brief	Closes the given widget.
		 */
		void close(EditorWidgetBase* widget);

		/**
		 * @brief	Retrieves the layout of all currently active widgets. You may later
		 * 			use this layout to restore exact position of the widgets.
		 */
		EditorWidgetLayoutPtr getLayout() const;

		/**
		 * @brief	Positions all widgets according to the provided layout. It will open
		 * 			new widgets or close current ones if needed.
		 */
		void setLayout(const EditorWidgetLayoutPtr& layout);

		/**
		 * @brief	Allows you to queue up widgets that will be registered as soon as an instance of EditorWidgetManager is
		 * 			created.
		 *
		 * @note	Useful primarily when widgets are being registered from static methods, because then there is no
		 * 			EditorWidgetManager instance yet.
		 */
		static void preRegisterWidget(const String& name, std::function<EditorWidgetBase*(EditorWidgetContainer&)> createCallback);

	private:
		/**
		 * @brief	Called whenever a pointer (e.g. mouse cursor) is moved.
		 */
		void onPointerMoved(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is released.
		 */
		void onPointerReleased(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is pressed.
		 */
		void onPointerPressed(const PointerEvent& event);

		Map<String, EditorWidgetBase*> mActiveWidgets;
		Map<String, std::function<EditorWidgetBase*(EditorWidgetContainer&)>> mCreateCallbacks;

		HEvent mOnPointerPressedConn;

		static Stack<std::pair<String, std::function<EditorWidgetBase*(EditorWidgetContainer&)>>> QueuedCreateCallbacks;
	};
}