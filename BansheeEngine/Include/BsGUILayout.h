#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayout : public GUIElementBase
	{
	public:
		GUILayout();
		virtual ~GUILayout();

		void addElement(GUIElement* element);
		void removeElement(GUIElement* element);
		void insertElement(CM::UINT32 idx, GUIElement* element);

		GUILayout& addLayoutX() { return addLayoutXInternal(this); }
		GUILayout& addLayoutY() { return addLayoutYInternal(this); }
		void removeLayout(GUILayout& layout) { removeLayoutInternal(layout); }
		GUILayout& insertLayoutX(CM::UINT32 idx) { return insertLayoutXInternal(this, idx); }
		GUILayout& insertLayoutY(CM::UINT32 idx) { return insertLayoutYInternal(this, idx); }

		GUIFixedSpace& addSpace(CM::UINT32 size);
		void removeSpace(GUIFixedSpace& space);
		GUIFixedSpace& insertSpace(CM::UINT32 idx, CM::UINT32 size);

		GUIFlexibleSpace& addFlexibleSpace();
		void removeFlexibleSpace(GUIFlexibleSpace& space);
		GUIFlexibleSpace& insertFlexibleSpace(CM::UINT32 idx);

		CM::UINT32 getNumChildren() const { return (CM::UINT32)mChildren.size(); }
		void removeChildAt(CM::UINT32 idx);

		CM::Vector2I _getOptimalSize() const { return CM::Vector2I(mOptimalWidth, mOptimalHeight); }
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
		CM::UINT32 _getActualWidth() const { return mActualWidth; }

		/**
		 * @brief	Gets an actual height of all the child elements in the layout.
		 *
		 * @note	updateLayoutInternal must be called whenever layout or its children change,
		 * 			otherwise this method will return an incorrect value.
		 * 			
		 *			Returned value is based on non-clipped element bounds.
		 */
		CM::UINT32 _getActualHeight() const { return mActualHeight; }
	protected:
		CM::Vector<CM::Vector2I>::type mOptimalSizes;
		CM::UINT32 mOptimalWidth;
		CM::UINT32 mOptimalHeight;

		CM::UINT32 mActualWidth;
		CM::UINT32 mActualHeight;
	};
}