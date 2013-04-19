#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsTextSprite.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIWindowFrame : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIWindowFrame* create(GUIWidget* parent, const SpriteTexturePtr& texture);
	protected:
		~GUIWindowFrame();

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
		virtual void fillBuffer(CM::Vector2* vertices, CM::Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const;
	private:
		ImageSprite* mImageSprite;
		SpriteTexturePtr mTexture;

		GUIWindowFrame(GUIWidget* parent, const SpriteTexturePtr& texture);
	};
}