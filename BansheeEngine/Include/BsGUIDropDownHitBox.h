#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * Helper class used for detecting when a certain area is in focus,
	 * and getting notified when that state changes.
	 */
	class GUIDropDownHitBox : public GUIElementContainer
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Creates a new drop down hit box that will detect mouse input over certain area.
		 * You must call "setBounds" to define the area.
		 *
		 * @param	captureMouse	If true mouse clicks will be captured by this control and wont be passed
		 *							to other GUI elements.
		 */
		static GUIDropDownHitBox* create(bool captureMouse);

		/**
		 * Creates a new drop down hit box that will detect mouse input over certain area.
		 * You must call "setBounds" to define the area.
		 *
		 * @param	captureMouse	If true mouse clicks will be captured by this control and wont be passed
		 *							to other GUI elements.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 */
		static GUIDropDownHitBox* create(bool captureMouse, const GUIOptions& layoutOptions);

		/**
		 * Sets a single rectangle bounds in which the hitbox will capture mouse events.
		 */
		void setBounds(const Rect2I& bounds) { mBounds.clear(); mBounds.push_back(bounds); }

		/**
		 * Sets complex bounds consisting of multiple rectangles in which the hitbox will capture mouse events.
		 */
		void setBounds(const Vector<Rect2I>& bounds) { mBounds = bounds; }

		/**
		 * Triggered when hit box loses focus (e.g. user clicks outside of its bounds).
		 */
		Event<void()> onFocusLost;

		/**
		 * Triggered when hit box gains focus (e.g. user clicks inside of its bounds).
		 */
		Event<void()> onFocusGained;

	private:
		GUIDropDownHitBox(bool captureMouse, const GUIDimensions& layoutOptions);

		virtual bool _commandEvent(const GUICommandEvent& ev);
		virtual bool _mouseEvent(const GUIMouseEvent& ev);
		virtual bool _isInBounds(const Vector2I position) const;

		Vector<Rect2I> mBounds;
		bool mCaptureMouse;
	};
}