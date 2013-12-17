#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILabel : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUILabel* create(GUIWidget& parent, const CM::HString& text, const GUIElementStyle* style = nullptr);
		static GUILabel* create(GUIWidget& parent, const CM::HString& text, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		static GUILabel* create(GUIWidget& parent, const GUIContent& content, const GUIElementStyle* style = nullptr);
		static GUILabel* create(GUIWidget& parent, const GUIContent& content, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		void setContent(const GUIContent& content);

	protected:
		~GUILabel();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

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

		virtual CM::Vector2I _getOptimalSize() const;
	private:
		TextSprite* mTextSprite;
		GUIContent mContent;
		boost::signals::connection mLocStringUpdatedConn;

		TEXT_SPRITE_DESC mDesc;
		
		GUILabel(GUIWidget& parent, const GUIElementStyle* style, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}