#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIInputBox : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIInputBox* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIInputBox* create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);
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
		ImageSprite* mImageSprite;
		ImageSprite* mCaretSprite;
		TextSprite* mTextSprite;
		CM::Vector<ImageSprite*>::type mSelectionSprites;
		CM::UINT32 mNumImageRenderElements;
		bool mInputCursorSet;
		bool mDragInProgress;

		CM::UINT32 mSelectionStart;
		CM::UINT32 mSelectionEnd;
		CM::UINT32 mCaretPos;
		bool mCaretShown;
		bool mSelectionShown;

		IMAGE_SPRITE_DESC mImageDesc;
		CM::WString mText;

		GUIInputBox(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool buttonEvent(const GUIButtonEvent& ev);
		virtual bool commandEvent(const GUICommandEvent& ev);

		Sprite* renderElemToSprite(CM::UINT32 renderElemIdx, CM::UINT32& localRenderElemIdx) const;

		void showCaret(CM::UINT32 charIdx);
		void clearCaret();
		CM::Int2 getCaretPosition() const;
		CM::UINT32 getCaretHeight() const;

		void showSelection(CM::UINT32 startChar, CM::UINT32 endChar);
		void clearSelection();
		CM::Vector<CM::Rect>::type getSelectionRects() const;

		CM::UINT32 getCharAtPosition(const CM::Int2& pos) const;
	};
}