#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	enum class GUIImageScaleMode
	{
		StretchToFit,
		ScaleToFit,
		CropToFit,
		RepeatToFit
	};

	class BS_EXPORT GUITexture : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITexture* create(const HSpriteTexture& texture, GUIImageScaleMode scale, 
			const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
		static GUITexture* create(const HSpriteTexture& texture, GUIImageScaleMode scale, 
			const GUIElementStyle* style = nullptr);

		static GUITexture* create(const HSpriteTexture& texture, 
			const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
		static GUITexture* create(const HSpriteTexture& texture, const GUIElementStyle* style = nullptr);

		
		static GUITexture* create(GUIImageScaleMode scale, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
		static GUITexture* create(GUIImageScaleMode scale, const GUIElementStyle* style = nullptr);

		static GUITexture* create(const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
		static GUITexture* create(const GUIElementStyle* style = nullptr);

		void setTexture(const HSpriteTexture& texture);
		
		virtual ElementType getElementType() const { return ElementType::Texture; }

		virtual CM::Vector2I _getOptimalSize() const;
	protected:
		ImageSprite* mImageSprite;
		HSpriteTexture mActiveTexture;
		IMAGE_SPRITE_DESC mDesc;
		GUIImageScaleMode mScaleMode;
		bool mUsingStyleTexture;

		GUITexture(const GUIElementStyle* style, const HSpriteTexture& texture, GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions);
		virtual ~GUITexture();

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
	};
}