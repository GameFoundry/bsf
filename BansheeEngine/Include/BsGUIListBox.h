#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIListBox : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIListBox* create(GUIWidget& parent, const CM::Vector<CM::WString>::type& elements, const GUIElementStyle* style = nullptr);
		static GUIListBox* create(GUIWidget& parent, const CM::Vector<CM::WString>::type& elements, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	
		boost::signal<void(CM::UINT32)> onSelectionChanged;
	protected:
		~GUIListBox();

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

		/**
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		virtual CM::Int2 _getOptimalSize() const;

		virtual CM::UINT32 _getRenderElementDepth(CM::UINT32 renderElementIdx) const;
	private:
		ImageSprite* mImageSprite;
		TextSprite* mTextSprite;
		CM::UINT32 mNumImageRenderElements;
		CM::UINT32 mSelectedIdx;

		IMAGE_SPRITE_DESC mImageDesc;
		CM::Vector<CM::WString>::type mElements;
		bool mIsListBoxOpen;

		GUIListBox(GUIWidget& parent, const GUIElementStyle* style, const CM::Vector<CM::WString>::type& elements, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		TEXT_SPRITE_DESC getTextDesc() const;

		void elementSelected(CM::UINT32 idx);

		void openListBox();
		void closeListBox();

		void onListBoxClosed();
	};
}