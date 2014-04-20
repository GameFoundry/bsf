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
		static const String& getGUITypeName();

		static GUITexture* create(const HSpriteTexture& texture, GUIImageScaleMode scale, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
		static GUITexture* create(const HSpriteTexture& texture, GUIImageScaleMode scale, 
			const String& styleName = StringUtil::BLANK);

		static GUITexture* create(const HSpriteTexture& texture, 
			const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
		static GUITexture* create(const HSpriteTexture& texture, 
			const String& styleName = StringUtil::BLANK);

		static GUITexture* create(GUIImageScaleMode scale, const GUIOptions& layoutOptions, 
			const String& styleName = StringUtil::BLANK);
		static GUITexture* create(GUIImageScaleMode scale, const String& styleName = StringUtil::BLANK);

		static GUITexture* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
		static GUITexture* create(const String& styleName = StringUtil::BLANK);

		void setTexture(const HSpriteTexture& texture);
		
		virtual ElementType getElementType() const { return ElementType::Texture; }

		virtual Vector2I _getOptimalSize() const;
	protected:
		ImageSprite* mImageSprite;
		HSpriteTexture mActiveTexture;
		IMAGE_SPRITE_DESC mDesc;
		GUIImageScaleMode mScaleMode;
		bool mUsingStyleTexture;

		GUITexture(const String& styleName, const HSpriteTexture& texture, GUIImageScaleMode scale, const GUILayoutOptions& layoutOptions);
		virtual ~GUITexture();

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
	};
}