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

		static GUITexture* create(GUIWidget& parent, const SpriteTexturePtr& texture, GUIImageScaleMode scale, const GUIElementStyle* style = nullptr);
		static GUITexture* create(GUIWidget& parent, const SpriteTexturePtr& texture, GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		~GUITexture();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const CM::HMaterial& getMaterial(UINT32 renderElementIdx) const;

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

		virtual UINT32 _getOptimalWidth() const;
		virtual UINT32 _getOptimalHeight() const;

	private:
		ImageSprite* mImageSprite;
		IMAGE_SPRITE_DESC mDesc;
		GUIImageScaleMode mScaleMode;

		GUITexture(GUIWidget& parent, const GUIElementStyle* style, const SpriteTexturePtr& texture, GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions);
	};
}