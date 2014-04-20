#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	enum class GUIButtonState
	{
		Normal = 0x01,
		Hover = 0x02,
		Active = 0x04,
		Focused = 0x08,
		NormalOn = 0x11,
		HoverOn = 0x12,
		ActiveOn = 0x14,
		FocusedOn = 0x18
	};

	class BS_EXPORT GUIButtonBase : public GUIElement
	{
	public:
		void setContent(const GUIContent& content);

		void _setOn(bool on);
		bool _isOn() const;

		virtual Vector2I _getOptimalSize() const;

		boost::signal<void()> onClick;
		boost::signal<void()> onHover;
		boost::signal<void()> onOut;
	protected:
		GUIButtonBase(const String& styleName, const GUIContent& content, const GUILayoutOptions& layoutOptions);
		virtual ~GUIButtonBase();

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

		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const;

		TEXT_SPRITE_DESC getTextDesc() const;

		void setState(GUIButtonState state);
		GUIButtonState getState() const { return mActiveState; }

		const HSpriteTexture& getActiveTexture() const;
	private:
		ImageSprite* mImageSprite;
		ImageSprite* mContentImageSprite;
		TextSprite* mTextSprite;
		GUIButtonState mActiveState;

		IMAGE_SPRITE_DESC mImageDesc;
		GUIContent mContent;

		boost::signals::connection mLocStringUpdatedConn;
	};
}