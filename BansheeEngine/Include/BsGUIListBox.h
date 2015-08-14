#pragma once

#include "BsPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	List box GUI element which when active opens a drop down
	 *			selection with provided elements.
	 */
	class BS_EXPORT GUIListBox : public GUIButtonBase
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new listbox with the provided elements.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBox* create(const Vector<HString>& elements, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new listbox with the provided elements.
		 *
		 * @param	elements		Elements to display in the list box.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIListBox* create(const Vector<HString>& elements, const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Changes the list box elements.
		 */
		void setElements(const Vector<HString>& elements);

		/**
		 * @brief	Makes the element with the specified index selected.
		 */
		void selectElement(UINT32 idx);

		/**
		 * @copydoc	GUIButtonBase::getElementType
		 */
		virtual ElementType _getElementType() const override { return ElementType::ListBox; }

		/**
		 * @brief	Triggered whenever user selects a new element in the list box. Returned index
		 *			maps to the element in the elements array that the list box was initialized with.
		 */
		Event<void(UINT32)> onSelectionChanged;
	protected:
		~GUIListBox();

	private:
		GUIListBox(const String& styleName, const Vector<HString>& elements, const GUIDimensions& dimensions);

		/**
		 * @copydoc	GUIButtonBase::mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @brief	Triggered when user clicks on an element.
		 */
		void elementSelected(UINT32 idx);

		/**
		 * @brief	Opens the list box drop down menu.
		 */
		void openListBox();

		/**
		 * @brief	Closes the list box drop down menu.
		 */
		void closeListBox();

		/**
		 * @brief	Called when the list box drop down menu is closed
		 *			by external influence.
		 */
		void onListBoxClosed();

	private:
		UINT32 mSelectedIdx;
		Vector<HString> mElements;
		bool mIsListBoxOpen;
	};
}