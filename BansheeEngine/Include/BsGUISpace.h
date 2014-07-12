#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element that may be inserted into layouts in order to make
	 *			a space of a fixed size.
	 */
	class BS_EXPORT GUIFixedSpace : public GUIElementBase
	{
	public:
		GUIFixedSpace(UINT32 size)
			:mSize(size)
		{
			_markAsClean();
		}

		/**
		 * @brief	Returns the size of the space in pixels.
		 */
		UINT32 getSize() const { return mSize; }

		/**
		 * @brief	Changes the size of the space to the specified value, in pixels.
		 */
		void setSize(UINT32 size) { mSize = size; markContentAsDirty(); }

		/**
		 * @copydoc	GUIElementBase::_getType
		 */
		Type _getType() const { return GUIElementBase::Type::FixedSpace; }

		/**
		 * @copydoc	GUIElementBase::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const { return Vector2I(getSize(), getSize()); }

		/**
		 * @copydoc	GUIElementBase::_getPadding
		 */
		virtual const RectOffset& _getPadding() const 
		{
			static RectOffset padding;

			return padding;
		}

	protected:
		UINT32 mSize;
	};

	/**
	 * @brief	GUI element that may be inserted into layouts to make a space of
	 *			a flexible size. The space will expand only if there is room and
	 *			other elements are not squished because of it. If multiple flexible
	 *			spaces are in a layout, their sizes will be shared equally.
	 *
	 * @note	e.g. If you had a horizontal layout with a button, and you wanted to
	 *			align that button to the right of the layout, you would insert a flexible
	 *			space before the button in the layout.
	 */
	class BS_EXPORT GUIFlexibleSpace : public GUIElementBase
	{
	public:
		GUIFlexibleSpace() 
		{
			_markAsClean();
		}

		/**
		 * @copydoc	GUIElementBase::_getType
		 */
		Type _getType() const { return GUIElementBase::Type::FlexibleSpace; }

		/**
		 * @copydoc	GUIElementBase::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const { return Vector2I(0, 0); }

		/**
		 * @copydoc	GUIElementBase::_getPadding
		 */
		virtual const RectOffset& _getPadding() const 
		{
			static RectOffset padding;

			return padding;
		}
	};
}