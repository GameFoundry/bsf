#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputBox : public GUIElement
	{
		enum class State
		{
			Normal,
			Hover,
			Focused
		};

	public:
		static const String& getGUITypeName();

		static GUIInputBox* create(bool multiline = false, const String& styleName = StringUtil::BLANK);
		static GUIInputBox* create(bool multiline, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
		static GUIInputBox* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		const WString& getText() const { return mText; }
		void setText(const WString& text);

		void setFilter(std::function<bool(const WString&)> filter) { mFilter = filter; }

		virtual ElementType getElementType() const { return ElementType::InputBox; }

		virtual Vector2I _getOptimalSize() const;
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

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool textInputEvent(const GUITextInputEvent& ev);
		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool virtualButtonEvent(const GUIVirtualButtonEvent& ev);

		virtual Vector2I _getTextInputOffset() const;
		virtual RectI _getTextInputRect() const;

		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;
		virtual bool _hasCustomCursor(const Vector2I position, CursorType& type) const;

		virtual GUIContextMenu* getContextMenu() const;
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

		Sprite* renderElemToSprite(UINT32 renderElemIdx, UINT32& localRenderElemIdx) const;
		Vector2I renderElemToOffset(UINT32 renderElemIdx) const;
		RectI renderElemToClipRect(UINT32 renderElemIdx) const;

		void insertString(UINT32 charIdx, const WString& string);
		void insertChar(UINT32 charIdx, UINT32 charCode);
		void eraseChar(UINT32 charIdx);
		void deleteSelectedText();
		WString getSelectedText();

		void showCaret();
		void hideCaret();

		void showSelection(UINT32 anchorCaretPos);
		void clearSelection();

		void moveSelectionLeft(bool skipNewline);
		void moveSelectionRight(bool skipNewline);

		void scrollTextToCaret();
		void clampScrollToBounds(RectI unclippedTextBounds);

		Vector2I getTextOffset() const;
		RectI getTextClipRect() const;
		TEXT_SPRITE_DESC getTextDesc() const;
		const HSpriteTexture& getActiveTexture() const;
		
		void cutText();
		void copyText();
		void pasteText();
	};
}