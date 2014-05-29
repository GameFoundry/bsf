#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayout : public GUIElementBase
	{
	public:
		GUILayout();
		virtual ~GUILayout();

		void addElement(GUIElement* element);
		void removeElement(GUIElement* element);
		void insertElement(UINT32 idx, GUIElement* element);

		GUILayout& addLayoutX() { return addLayoutXInternal(this); }
		GUILayout& addLayoutY() { return addLayoutYInternal(this); }
		void removeLayout(GUILayout& layout) { removeLayoutInternal(layout); }
		GUILayout& insertLayoutX(UINT32 idx) { return insertLayoutXInternal(this, idx); }
		GUILayout& insertLayoutY(UINT32 idx) { return insertLayoutYInternal(this, idx); }

		GUIFixedSpace& addSpace(UINT32 size);
		void removeSpace(GUIFixedSpace& space);
		GUIFixedSpace& insertSpace(UINT32 idx, UINT32 size);

		GUIFlexibleSpace& addFlexibleSpace();
		void removeFlexibleSpace(GUIFlexibleSpace& space);
		GUIFlexibleSpace& insertFlexibleSpace(UINT32 idx);

		UINT32 getNumChildren() const { return (UINT32)mChildren.size(); }
		void removeChildAt(UINT32 idx);

		Vector2I _getOptimalSize() const { return Vector2I(mOptimalWidth, mOptimalHeight); }
		const RectOffset& _getPadding() const;
		Type _getType() const { return GUIElementBase::Type::Layout; }

		/**
		 * @brief	Gets an actual width of all the child elements in the layout.
		 *
		 * @note	updateLayoutInternal must be called whenever layout or its children change,
		 * 			otherwise this method will return an incorrect value.
		 * 			
		 *			Returned value is based on non-clipped element bounds.
		 */
		UINT32 _getActualWidth() const { return mActualWidth; }

		/**
		 * @brief	Gets an actual height of all the child elements in the layout.
		 *
		 * @note	updateLayoutInternal must be called whenever layout or its children change,
		 * 			otherwise this method will return an incorrect value.
		 * 			
		 *			Returned value is based on non-clipped element bounds.
		 */
		UINT32 _getActualHeight() const { return mActualHeight; }
	protected:
		Vector<Vector2I> mOptimalSizes;
		UINT32 mOptimalWidth;
		UINT32 mOptimalHeight;

		UINT32 mActualWidth;
		UINT32 mActualHeight;
	};
}