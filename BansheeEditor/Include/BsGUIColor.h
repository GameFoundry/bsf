#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	class GUIColor : public GUIElement
	{
	public:
		static const String& getGUITypeName();

		static GUIColor* create(const String& styleName = StringUtil::BLANK);
		static GUIColor* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		virtual Vector2I _getOptimalSize() const;

		void setColor(const Color& color);
		Color getColor() const { return mColor; }

	protected:
		GUIColor(const String& styleName, const GUILayoutOptions& layoutOptions);
		virtual ~GUIColor();

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

	private:
		static const float ALPHA_SPLIT_POSITION;

		ImageSprite* mColorSprite;
		ImageSprite* mAlphaSprite;

		IMAGE_SPRITE_DESC mColorImageDesc;
		IMAGE_SPRITE_DESC mAlphaImageDesc;

		Color mColor;
	};
}