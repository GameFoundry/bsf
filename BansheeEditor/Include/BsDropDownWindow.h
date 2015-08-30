#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	This is a generic GUI drop down window class that 
	 *			can be used for displaying custom drop down content.
	 */
	class BS_ED_EXPORT DropDownWindow
	{
	public:
		virtual ~DropDownWindow();

		/**
		 * @brief	Returns width of the window in pixels.
		 */
		UINT32 getWidth() const { return mWidth; }
		
		/**
		 * @brief	Returns height of the window in pixels.
		 */
		UINT32 getHeight() const { return mHeight; }

		/**
		 * @brief	Converts screen pointer coordinates into coordinates relative to the window contents GUI panel.
		 */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @brief	Converts pointer coordinates relative to the window contents GUI panel into screen coordinates.
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @brief	Returns the GUI widget used for displaying all GUI contents in the window.
		 */
		HGUIWidget getGUIWidget() const { return mGUI; }

		/**
		 * @brief	Changes the size of the drop down window area.
		 *			
		 * @note	This might reposition the window if the new size doesn't fit at the current position.
		 */
		void setSize(UINT32 width, UINT32 height);

		/**
		 * @brief	Closes the drop down window.
		 */
		void close();

		/**
		 * @brief	Called once every frame. Internal method.
		 */
		virtual void update() { }

	protected:
		DropDownWindow(const RenderWindowPtr& parent, const CameraPtr& camera,
			const Vector2I& position, UINT32 width = 200, UINT32 height = 200);

		GUIPanel* mContents;

	private:
		friend class DropDropWindowManager;

		/**
		 * @brief	Triggered when the user clicks outside of the drop down area.
		 */
		void dropDownFocusLost();

		RenderWindowPtr mRenderWindow;
		HSceneObject mSceneObject;
		HGUIWidget mGUI;

		GUIDropDownHitBox* mFrontHitBox;
		GUIDropDownHitBox* mBackHitBox;
		GUIPanel* mRootPanel;
		Vector2I mPosition;
		UINT32 mWidth;
		UINT32 mHeight;
	};
}