#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIInputCaret.h"

namespace BansheeEngine
{
	enum class SelectionDir
	{
		Left,
		Right
	};

	class BS_EXPORT GUIInputBox : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIInputBox* create(GUIWidget& parent, bool multiline = false, const GUIElementStyle* style = nullptr);
		static GUIInputBox* create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, bool multiline = false, const GUIElementStyle* style = nullptr);
	protected:
		~GUIInputBox();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const CM::HMaterial& getMaterial(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads()
		 */
		virtual CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer()
		 */
		virtual void fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, 
			CM::UINT32 maxNumQuads, CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();

		virtual CM::UINT32 _getOptimalWidth() const;
		virtual CM::UINT32 _getOptimalHeight() const;

		virtual CM::UINT32 _getRenderElementDepth(CM::UINT32 renderElementIdx) const;
		virtual void _setFocus(bool focus);
	private:
		// Sprites
		ImageSprite* mImageSprite;
		TextSprite* mTextSprite;
		CM::Vector<ImageSprite*>::type mSelectionSprites;
		GUIInputCaret* mInputCaret;
		bool mInputCursorSet;
		bool mDragInProgress;
		bool mIsMultiline;
		CM::Int2 mTextOffset;

		IMAGE_SPRITE_DESC mImageDesc;
		CM::WString mText;

		// Selection & input caret
		CM::UINT32 mSelectionStart;
		CM::UINT32 mSelectionEnd;
		CM::UINT32 mSelectionAnchor;
		bool mCaretShown;
		bool mSelectionShown;

		GUIInputBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions, bool multiline);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool keyEvent(const GUIKeyEvent& ev);
		virtual bool commandEvent(const GUICommandEvent& ev);

		Sprite* renderElemToSprite(CM::UINT32 renderElemIdx, CM::UINT32& localRenderElemIdx) const;

		void showCaret();
		void hideCaret();
		void scrollTextToCaret();

		void showSelection(CM::UINT32 startChar);
		void clearSelection();
		CM::UINT32 getCaretSelectionCharIdx(SelectionDir dir) const;
		bool isNewlineChar(CM::UINT32 charIdx) const;
		CM::Vector<CM::Rect>::type getSelectionRects() const;

		CM::Rect getTextBounds() const;
		TEXT_SPRITE_DESC getTextDesc() const;
	};
}