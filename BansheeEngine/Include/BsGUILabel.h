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
		static const String& getGUITypeName();

		static GUILabel* create(const HString& text, const String& styleName = StringUtil::BLANK);
		static GUILabel* create(const HString& text, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		static GUILabel* create(const GUIContent& content, const String& styleName = StringUtil::BLANK);
		static GUILabel* create(const GUIContent& content, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		void setContent(const GUIContent& content);

		virtual Vector2I _getOptimalSize() const;
		virtual ElementType getElementType() const { return ElementType::Label; }
	protected:
		~GUILabel();

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
	private:
		TextSprite* mTextSprite;
		GUIContent mContent;
		HEvent mLocStringUpdatedConn;

		TEXT_SPRITE_DESC mDesc;
		
		GUILabel(const String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions);
	};
}