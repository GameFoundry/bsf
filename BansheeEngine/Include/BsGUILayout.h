#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "CmInt2.h"

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

		GUILayout& addLayoutX() { return addLayoutXInternal(); }
		GUILayout& addLayoutY() { return addLayoutYInternal(); }
		void removeLayout(GUILayout& layout) { removeLayoutInternal(layout); }
		GUILayout& insertLayoutX(CM::UINT32 idx) { return insertLayoutXInternal(idx); }
		GUILayout& insertLayoutY(CM::UINT32 idx) { return insertLayoutYInternal(idx); }

		GUIFixedSpace& addSpace(CM::UINT32 size);
		void removeSpace(GUIFixedSpace& space);
		GUIFixedSpace& insertSpace(CM::UINT32 idx, CM::UINT32 size);

		GUIFlexibleSpace& addFlexibleSpace();
		void removeFlexibleSpace(GUIFlexibleSpace& space);
		GUIFlexibleSpace& insertFlexibleSpace(CM::UINT32 idx);

		/**
		 * @brief	Returns a number of all child elements
		 */
		CM::UINT32 getNumChildren() const;

		CM::UINT32 _getOptimalWidth() const { return mOptimalWidth; }
		CM::UINT32 _getOptimalHeight() const { return mOptimalHeight; }
		Type _getType() const { return GUIElementBase::Type::Layout; }

	protected:
		CM::Vector<CM::Int2>::type mOptimalSizes;
		CM::UINT32 mOptimalWidth;
		CM::UINT32 mOptimalHeight;
	};
}