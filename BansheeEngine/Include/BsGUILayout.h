#pragma once

#include "BsPrerequisites.h"

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
		void insertElement(UINT32 idx, GUIElement* element);

		GUILayout& addLayoutX();
		GUILayout& addLayoutY();
		void removeLayout(GUILayout& layout);
		GUILayout& insertLayoutX(UINT32 idx);
		GUILayout& insertLayoutY(UINT32 idx);

		GUIFixedSpace& addSpace(UINT32 size);
		void removeSpace(GUIFixedSpace& space);
		GUIFixedSpace& insertSpace(UINT32 idx, UINT32 size);

		GUIFlexibleSpace& addFlexibleSpace();
		void removeFlexibleSpace(GUIFlexibleSpace& space);
		GUIFlexibleSpace& insertFlexibleSpace(UINT32 idx);

		/**
		 * @brief	Returns a combined number of child elements and layouts.
		 */
		UINT32 getNumChildren() const;

		/**
		 * @brief	Re-arranges the elements to fit the layout. You shouldn't need to call this manually
		 */
		void _update(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth);

		bool _isDirty();

	protected:
		std::vector<GUILayoutEntry> mChildren;		
		bool mIsDirty;

		virtual void updateInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth) = 0;
	};
}