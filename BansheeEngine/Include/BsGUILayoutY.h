#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayout.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a vertical layout that will layout out its child elements
	 *			top to bottom.
	 */
	class BS_EXPORT GUILayoutY : public GUILayout
	{
	public:
		GUILayoutY(const GUIDimensions& dimensions);
		GUILayoutY() {};
		~GUILayoutY() {};

		/**
		 * @brief	Calculate optimal sizes of all child layout elements.
		 */
		void _updateOptimalLayoutSizes() override;

		/**
		 * @copydoc	GUIElementBase::_calculateLayoutSizeRange
		 */
		virtual LayoutSizeRange _calculateLayoutSizeRange() const override;

		/**
		 * @copydoc	GUILayout::_getElementAreas
		 */
		void _getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements,
			const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const override;

		/**
		 * @copydoc	GUILayout::_calcActualSize
		 */
		 virtual Vector2I _calcActualSize(INT32 x, INT32 y, Rect2I* elementAreas, UINT32 numElements) const override;

		/**
		 * @brief	Creates a new vertical layout.
		 */
		static GUILayoutY* create();

		/**
		 * @brief	Creates a new vertical layout.
		 *
		 * @param	options		Options that allow you to control how is the element positioned and sized.
		 */
		static GUILayoutY* create(const GUIOptions& options);

	protected:
		/**
		 * @copydoc	GUIElementBase::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;
	};
}