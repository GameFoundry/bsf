#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsGUIDropDownBox.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element that is used for representing entries in a drop down menu.
	 */
	class BS_EXPORT GUIDropDownContent : public GUIElementContainer
	{
		/**
		 * @brief	Contains various GUI elements used for displaying a single menu entry.
		 */
		struct VisibleElement
		{
			UINT32 idx = 0;
			GUIButton* button = nullptr;
			GUITexture* separator = nullptr;
			GUILabel* shortcutLabel = nullptr;
		};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new drop down contents element.
		 *
		 * @param	parent			Parent sub-menu that owns the drop down contents.
		 * @param	dropDownData	Data that will be used for initializing the child entries.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default button style is used.
		 */
		static GUIDropDownContent* create(GUIDropDownBox::DropDownSubMenu* parent, const GUIDropDownData& dropDownData,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new drop down contents element.
		 *
		 * @param	parent			Parent sub-menu that owns the drop down contents.
		 * @param	dropDownData	Data that will be used for initializing the child entries.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default button style is used.
		 */
		static GUIDropDownContent* create(GUIDropDownBox::DropDownSubMenu* parent, const GUIDropDownData& dropDownData, 
			const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Changes the range of the displayed elements. 
		 *
		 * @note	This must be called at least once after creation.
		 */
		void setRange(UINT32 start, UINT32 end);

		/**
		 * @brief	Returns height of a menu element at the specified index, in pixels.
		 */
		UINT32 getElementHeight(UINT32 idx) const;

		/**
		 * @brief	Enables or disables keyboard focus. When keyboard focus is enabled the contents
		 *			will respond to keyboard events.
		 */
		void setKeyboardFocus(bool focus);

		static const String ENTRY_STYLE_TYPE;
		static const String ENTRY_EXP_STYLE_TYPE;
		static const String SEPARATOR_STYLE_TYPE;
	protected:
		GUIDropDownContent(GUIDropDownBox::DropDownSubMenu* parent, const GUIDropDownData& dropDownData, 
			const String& style, const GUIDimensions& dimensions);
		~GUIDropDownContent() override;

		/**
		 * @brief	Get localized name of a menu item element with the specified index.
		 */
		HString getElementLocalizedName(UINT32 idx) const;

		/**
		 * @copydoc	GUIElementContainer::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @copydoc	GUIElementContainer::updateClippedBounds
		 */
		void updateClippedBounds() override;

		/**
		 * @copydoc	GUIElementContainer::_updateLayoutInternal
		 */
		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax) override;

		/**
		 * @copydoc	GUIElementContainer::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @copydoc	GUIElementContainer::_commandEvent
		 */
		bool _commandEvent(const GUICommandEvent& ev) override;

		/**
		 * @brief	Marks the element with the specified index as selected.
		 */
		void setSelected(UINT32 idx);

		/**
		 * @brief	Selects the next available non-separator entry.
		 */
		void selectNext(UINT32 startIdx);

		/**
		 * @brief	Selects the previous available non-separator entry.
		 */
		void selectPrevious(UINT32 startIdx);

		GUIDropDownData mDropDownData;
		Vector<VisibleElement> mVisibleElements;
		UINT32 mSelectedIdx;
		UINT32 mRangeStart, mRangeEnd;
		GUIDropDownBox::DropDownSubMenu* mParent;
		bool mKeyboardFocus;
	};
}