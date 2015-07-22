#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIToggle.h"
#include "BsGUIToggleGroup.h"
#include "BsImageSprite.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Specialization of a GUIToggle element used for displaying
	 *			tabs in a GUITabbedTitleBar. Aside from being toggleable these
	 *			buttons also track drag and drop operations.
	 */
	class GUITabButton : public GUIToggle
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new GUI tab button element.
		 *
		 * @param	toggleGroup		A toggle group that forms a link between related tab buttons.
		 * @param	index			Unique index that can be used for identifying a tab.
		 * @param	text			Label to display in the button.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const HString& text, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI tab button element.
		 *
		 * @param	toggleGroup		A toggle group that forms a link between related tab buttons.
		 * @param	index			Unique index that can be used for identifying a tab.
		 * @param	text			Label to display in the button.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const HString& text, 
			const GUIOptions& options, const String& styleName = StringUtil::BLANK);
	
		/**
		 * @brief	Creates a new GUI tab button element.
		 *
		 * @param	toggleGroup		A toggle group that forms a link between related tab buttons.
		 * @param	index			Unique index that can be used for identifying a tab.
		 * @param	content			Content to display in the button.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, 
			const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI tab button element.
		 *
		 * @param	toggleGroup		A toggle group that forms a link between related tab buttons.
		 * @param	index			Unique index that can be used for identifying a tab.
		 * @param	content			Content to display in the button.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITabButton* create(const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, 
			const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Returns the unique index for this tab button.
		 */
		UINT32 getIndex() const { return mIndex; }

		/**
		 * @brief	Changes the button state to dragged or not dragged, resulting primarily
		 *			in a visual change.
		 *
		 * @note	Internal method.
		 */
		void _setDraggedState(bool active);

		/**
		 * @brief	Triggered when the user starts dragging the tab button. Reported parameters
		 *			are the unique index of the tab and pointer position relative to parent GUIWidget.
		 */
		Event<void(UINT32, const Vector2I&)> onDragged; 

		/**
		 * @brief	Triggered when the user ends dragging the tab button. Reported parameters
		 *			are the unique index of the tab and pointer position relative to parent GUIWidget.
		 */
		Event<void(UINT32, const Vector2I&)> onDragEnd;
	protected:
		GUITabButton(const String& styleName, const GUIToggleGroupPtr& toggleGroup, UINT32 index, const GUIContent& content, const GUIDimensions& dimensions);

		/**
		 * @copydoc GUIElement::_mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		UINT32 mIndex;
		Vector2I mDragStartPosition;
		bool mDraggedState;
		GUIButtonState mInactiveState;

		static const UINT32 DRAG_MIN_DISTANCE;
	};
}