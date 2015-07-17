#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	A base class for all editor window. Each editor window is backed by a render window
	 *			(e.g. the OS "window"), and also provides a GUI widget for child elements to use.
	 *			On top of that it also provides basic GUI like window background and frame, as well
	 *			as setting up the OS-specific areas for resize/move operations.
	 */
	class BS_ED_EXPORT EditorWindowBase
	{
	public:
		virtual ~EditorWindowBase();

		/**
		 * @brief	Moves the window on the desktop. Coordinates are in screen space.
		 */
		virtual void setPosition(INT32 x, INT32 y);

		/**
		 * @brief	Resizes the window.
		 */
		virtual void setSize(UINT32 width, UINT32 height);

		/**
	     * @brief	Returns the X position of the window in screen coordinates.
		 */
		INT32 getLeft() const;

		/**
	     * @brief	Returns the X position of the window in screen coordinates.
		 */
		INT32 getTop() const;

		/**
	     * @brief	Returns the width of the window in pixels.
		 */
		UINT32 getWidth() const;

		/**
	     * @brief	Returns the height of the window in pixels.
		 */
		UINT32 getHeight() const;

		/**
	     * @brief	Closes and destroys the window.
		 */
		virtual void close();

		/**
	     * @brief	Hides the window without closing it.
		 */
		void hide();

		/**
		 * @brief	Return true if this is the main editor window.
		 */
		virtual bool isMain() const { return false; }

		/**
		 * @brief	Called once every frame. Internal method.
		 */
		virtual void update() { }

		/**
		 * @brief	Returns the render window that this editor window is being rendered to.
		 */
		RenderWindowPtr getRenderWindow() const { return mRenderWindow; }

		/**
		 * @brief	Returns the GUI widget used for displaying all GUI contents in the window.
		 */
		HGUIWidget getGUIWidget() const { return mGUI; }

	protected:
		EditorWindowBase(bool isModal = false);
		EditorWindowBase(const RenderWindowPtr& renderWindow);

		RenderWindowPtr mRenderWindow;
		HSceneObject mSceneObject;
		HGUIWidget mGUI;
		HCamera mCamera;
		GameObjectHandle<WindowFrameWidget> mWindowFrame;
		bool mOwnsRenderWindow;
		bool mIsModal;

		/**
		 * @brief	Common code for constructing the object to be called from all constructors.
		 */
		void construct(const RenderWindowPtr& renderWindow);

		/**
		 * @brief	Initializes elements that cannot be initialized in the constructor. Must be called right after construction.
		 */
		virtual void initialize();

		/**
		 * @brief	Callback that triggers whenever the underlying render window changes size.
		 */
		virtual void resized() { }
	private:
		HEvent mResizedConn;
	};
}