#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	/**
	 * @brief	Input box is a GUI element at accepts Unicode textual input. It can be
	 *			single or multi-line and handles various types of text manipulation.
	 */
	class BS_EXPORT GUIInputBox : public GUIElement
	{
		/**
		 * @brief	Possible visual states the input box can be in.
		 */
		enum class State
		{
			Normal,
			Hover,
			Focused
		};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * Creates a new input box.
		 *
		 * @param	multiline		If true the input box can be of arbitrary height and will accept multiple lines of text.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style for this element is used.
		 */
		static GUIInputBox* create(bool multiline = false, const String& styleName = StringUtil::BLANK);

		/**
		 * Creates a new input box.
		 *
		 * @param	multiline		If true the input box can be of arbitrary height and will accept multiple lines of text.
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default button style is used.
		 */
		static GUIInputBox* create(bool multiline, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);


		/**
		 * Creates a new single-line input box.
		 *
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default button style is used.
		 */
		static GUIInputBox* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Returns the text currently entered in the input box.
		 */
		const WString& getText() const { return mText; }

		/**
		 * @brief	Sets the text inside the input box. This will replace any current text.
		 */
		void setText(const WString& text);

		/**
		 * @brief	Sets an optional filter that can control what is allowed to be entered into the
		 *			input box. Filter should return true if the provided string is valid and false otherwise.
		 *			Set the filter to null to deactivate filtering.
		 */
		void setFilter(std::function<bool(const WString&)> filter) { mFilter = filter; }

		/**
		 * @copydoc	GUIElement::getElementType
		 */
		virtual ElementType getElementType() const { return ElementType::InputBox; }

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

		/**
		 * @brief	Triggered whenever input text has changed.
		 */
		Event<void(const WString&)> onValueChanged;

		/**
		 * @brief	Triggered whenever the input box receives focus.
		 */
		Event<void()> onFocusGained;

		/**
		 * @brief	Triggered whenever the input box loses focus.
		 */
		Event<void()> onFocusLost;
	protected:
		GUIInputBox(const String& styleName, const GUILayoutOptions& layoutOptions, bool multiline);
		virtual ~GUIInputBox();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc	GUIElement::mouseEvent
		 */
		virtual bool mouseEvent(const GUIMouseEvent& ev);

		/**
		 * @copydoc	GUIElement::textInputEvent
		 */
		virtual bool textInputEvent(const GUITextInputEvent& ev);

		/**
		 * @copydoc	GUIElement::commandEvent
		 */
		virtual bool commandEvent(const GUICommandEvent& ev);

		/**
		 * @copydoc	GUIElement::virtualButtonEvent
		 */
		virtual bool virtualButtonEvent(const GUIVirtualButtonEvent& ev);

		/**
		 * @brief	Returns how much to offset text due to scrolling.
		 *
		 * @note	This is used when text is larger than the input box itself. As the
		 *			caret moves the text will scroll so that the caret remains visible, and
		 *			how much scroll is applied is determined by this value.
		 */
		virtual Vector2I _getTextInputOffset() const;

		/**
		 * @brief	Returns rectangle in which the text can be displayed, in local
		 *			coordinates (i.e. text will start at 0, 0).
		 */
		virtual Rect2I _getTextInputRect() const;

		/**
		 * @copydoc	GUIElement::_getRenderElementDepth
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;

		/**
		 * @copydoc	GUIElement::_getRenderElementDepthRange
		 */
		virtual UINT32 _getRenderElementDepthRange() const;

		/**
		 * @copydoc	GUIElement::_hasCustomCursor
		 */
		virtual bool _hasCustomCursor(const Vector2I position, CursorType& type) const;

		/**
		 * @copydoc	GUIElement::getContextMenu
		 */
		virtual GUIContextMenu* getContextMenu() const;
	private:
		/**
		 * @brief	Retrieves a sprite from a render element index, and a local render element index
		 *			that represents render element within the returned sprite.
		 */
		Sprite* renderElemToSprite(UINT32 renderElemIdx, UINT32& localRenderElemIdx) const;

		/**
		 * @brief	Returns offset at which is the element with the provided render element index.
		 *			Offset is relative to parent widget.
		 */
		Vector2I renderElemToOffset(UINT32 renderElemIdx) const;

		/**
		 * @brief	Returns a clip rectangle that can be used for clipping the render element
		 *			with the provided index. Rectangle is in local coordiantes relative to
		 *			element origin.
		 */
		Rect2I renderElemToClipRect(UINT32 renderElemIdx) const;

		/**
		 * @brief	Inserts a new string into the current text at the specified index.
		 */
		void insertString(UINT32 charIdx, const WString& string);

		/**
		 * @brief	Inserts a new character into the current text at the specified index.
		 */
		void insertChar(UINT32 charIdx, UINT32 charCode);

		/**
		 * @brief	Erases a single character at the specified index.
		 */
		void eraseChar(UINT32 charIdx);

		/**
		 * @brief	Deletes text that is currently selected.
		 */
		void deleteSelectedText();

		/**
		 * @brief	Returns currently selected text.
		 */
		WString getSelectedText();

		/**
		 * @brief	Shows the input caret. You must position the caret manually after showing it.
		 */
		void showCaret();

		/**
		 * @brief	Hides the input caret.
		 */
		void hideCaret();

		/**
		 * @brief	Shows selection with the specified anchor position. You must
		 *			position selection start and end before selection will actually render.
		 *			Anchor position determines selection behavior as the user moves the selection
		 *			with the keyboard.
		 */
		void showSelection(UINT32 anchorCaretPos);

		/**
		 * @brief	Removes any active selection.
		 */
		void clearSelection();

		/**
		 * @brief	Adjusts the text offset (scroll) so that the caret is visible.
		 */
		void scrollTextToCaret();

		/**
		 * @brief	Clamps the text offset (scroll)	so that the text fits in the
		 *			provided bounds nicely with minimal white space.
		 */
		void clampScrollToBounds(Rect2I unclippedTextBounds);

		/**
		 * @brief	Returns offset at which to render the text. Relative to parent widget.
		 */
		Vector2I getTextOffset() const;

		/**
		 * @brief	Returns rectangle used for clipping the text. Relative to element.
		 */
		Rect2I getTextClipRect() const;

		/**
		 * @brief	Returns text sprite descriptor determining how is text sprite created.
		 */
		TEXT_SPRITE_DESC getTextDesc() const;

		/**
		 * @brief	Returns currently active input box texture, depending on active state.
		 */
		const HSpriteTexture& getActiveTexture() const;
		
		/**
		 * @brief	Cuts currently selected text to clipboard.
		 */
		void cutText();

		/**
		 * @brief	Copies currently selected text to clipboard.
		 */
		void copyText();

		/**
		 * @brief	Inserts text from clipboard to current caret location.
		 */
		void pasteText();

	private:
		static VirtualButton mCopyVB;
		static VirtualButton mPasteVB;
		static VirtualButton mCutVB;
		static VirtualButton mSelectAllVB;

		// Sprites
		ImageSprite* mImageSprite;
		TextSprite* mTextSprite;
		bool mIsMultiline;
		Vector2I mTextOffset;
		bool mHasFocus;
		bool mIsMouseOver;
		State mState;

		IMAGE_SPRITE_DESC mImageDesc;
		WString mText;
		std::function<bool(const WString&)> mFilter;

		bool mCaretShown;
		bool mSelectionShown;
		bool mDragInProgress;
	};
}