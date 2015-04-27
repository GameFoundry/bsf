#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for layout GUI element. Layout element positions and sizes
	 *			any child elements according to element styles and layout options.
	 */
	class BS_EXPORT GUILayout : public GUIElementBase
	{
	public:
		GUILayout(GUIArea* parentArea);
		GUILayout();
		virtual ~GUILayout();

		/**
		 * @brief	Creates a new element and adds it to the layout after all existing elements.
		 */
		template<class Type, class... Args>
		Type* addNewElement(Args &&...args)
		{
			Type* elem = Type::create(std::forward<Args>(args)...);
			addElement(elem);
			return elem;
		}

		/**
		 * @brief	Creates a new element and inserts it before the element at the specified index.
		 */
		template<class Type, class... Args>
		Type* insertNewElement(UINT32 idx, Args &&...args)
		{
			Type* elem = Type::create(std::forward<Args>(args)...);
			insertElement(idx, elem);
			return elem;
		}

		/**
		 * @brief	Adds a new element to the layout after all existing elements.
		 */
		void addElement(GUIElementBase* element);

		/**
		 * @brief	Removes the specified element from the layout.
		 */
		void removeElement(GUIElementBase* element);

		/**
		 * @brief	Removes a child element at the specified index.
		 */
		void removeElementAt(UINT32 idx);

		/**
		 * @brief	Inserts a GUI element before the element at the specified index.
		 */
		void insertElement(UINT32 idx, GUIElementBase* element);

		/**
		 * @brief	Returns number of child elements in the layout.
		 */
		UINT32 getNumChildren() const { return (UINT32)mChildren.size(); }

		/**
		 * @brief	Returns a size range that was cached during the last "_updateOptimalLayoutSizes" call.
		 */
		LayoutSizeRange _getCachedSizeRange() const { return mSizeRange; }

		/**
		 * @copydoc	GUIElementBase::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const { return mSizeRange.optimal; }

		/**
		 * @copydoc	GUIElementBase::_getPadding
		 */
		const RectOffset& _getPadding() const;

		/**
		 * @copydoc	GUIElementBase::_getType
		 */
		Type _getType() const { return GUIElementBase::Type::Layout; }

		/**
		 * @brief	Gets an actual width (in pixels) of all the child elements in the layout.
		 *
		 * @note	updateLayoutInternal must be called whenever layout or its children change,
		 * 			otherwise this method will return an incorrect value.
		 * 			
		 *			Returned value is based on non-clipped element bounds.
		 */
		UINT32 _getActualWidth() const { return mActualWidth; }

		/**
		 * @brief	Gets an actual height (in pixels) of all the child elements in the layout.
		 *
		 * @note	updateLayoutInternal must be called whenever layout or its children change,
		 * 			otherwise this method will return an incorrect value.
		 * 			
		 *			Returned value is based on non-clipped element bounds.
		 */
		UINT32 _getActualHeight() const { return mActualHeight; }

		/**
		 * @brief	Returns parent GUI area of this layout. This value is only be
		 *			non null if the layout is top level.
		 */
		GUIArea* _getParentGUIArea() const { return mParentGUIArea; }

		/**
		 * @brief	Calculates the actual size of the layout taken up by all of its elements.
		 *			
		 * @note	Actual size means the bounds might be smaller or larger than the layout area itself.
		 *			If larger that means certain portions of the child elements will be clipped, and if
		 *			smaller certain portions of the layout area will be empty.
		 */
		virtual Vector2I _calcActualSize(Rect2I* elementAreas, UINT32 numElements) const = 0;

		/**
		 * @brief	Destroy the layout. Removes it from parent and widget, and deletes it.
		 */	
		static void destroy(GUILayout* layout);

	protected:
		GUIArea* mParentGUIArea;

		Vector<LayoutSizeRange> mChildSizeRanges;
		LayoutSizeRange mSizeRange;

		UINT32 mActualWidth;
		UINT32 mActualHeight;
	};
}