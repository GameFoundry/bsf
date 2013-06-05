#pragma once

#include "BsPrerequisites.h"
#include "CmInt2.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayout
	{
		enum class Type
		{
			Layout,
			Element,
			FixedSpace,
			FlexibleSpace
		};

		struct GUILayoutEntry
		{
			union
			{
				GUIElement* element;
				GUILayout* layout;
				GUIFixedSpace* space;
				GUIFlexibleSpace* flexibleSpace;
			};

			bool isElement() const { return mType == Type::Element; }
			bool isLayout() const { return mType == Type::Layout; }
			bool isFixedSpace() const { return mType == Type::FixedSpace; }
			bool isFlexibleSpace() const { return mType == Type::FlexibleSpace; }

			void setElement(GUIElement* _element)
			{
				element = _element;
				mType = Type::Element;
			}

			void setLayout(GUILayout* _layout)
			{
				layout = _layout;
				mType = Type::Layout;
			}

			void setSpace(GUIFixedSpace* _space)
			{
				space = _space;
				mType = Type::FixedSpace;
			}

			void setFlexibleSpace(GUIFlexibleSpace* _space)
			{
				flexibleSpace = _space;
				mType = Type::FlexibleSpace;
			}

			Type mType;
		};

	public:
		GUILayout();
		virtual ~GUILayout();

		void addElement(GUIElement* element);
		void removeElement(GUIElement* element);
		void insertElement(CM::UINT32 idx, GUIElement* element);

		GUILayout& addLayoutX();
		GUILayout& addLayoutY();
		void removeLayout(GUILayout& layout);
		GUILayout& insertLayoutX(CM::UINT32 idx);
		GUILayout& insertLayoutY(CM::UINT32 idx);

		GUIFixedSpace& addSpace(CM::UINT32 size);
		void removeSpace(GUIFixedSpace& space);
		GUIFixedSpace& insertSpace(CM::UINT32 idx, CM::UINT32 size);

		GUIFlexibleSpace& addFlexibleSpace();
		void removeFlexibleSpace(GUIFlexibleSpace& space);
		GUIFlexibleSpace& insertFlexibleSpace(CM::UINT32 idx);

		/**
		 * @brief	Returns a combined number of child elements and layouts.
		 */
		CM::UINT32 getNumChildren() const;

		/**
		 * @brief	Re-arranges the elements to fit the layout. (Internal use only)
		 */
		void _update(CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::UINT32 _getOptimalWidth() const { return mOptimalWidth; }
		CM::UINT32 _getOptimalHeight() const { return mOptimalHeight; }

		void _markAsDirty() { mIsDirty = true; }
		bool _isDirty();

		virtual void _updateOptimalSizes() = 0;
	protected:
		CM::vector<GUILayoutEntry>::type mChildren;	
		CM::vector<CM::Int2>::type mOptimalSizes;
		CM::UINT32 mOptimalWidth;
		CM::UINT32 mOptimalHeight;

		bool mIsDirty;

		virtual void updateInternal(CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT8 widgetDepth, CM::UINT16 areaDepth) = 0;
	};
}