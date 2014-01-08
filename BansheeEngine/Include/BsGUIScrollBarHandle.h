#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarHandle : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIScrollBarHandle* create(GUIWidget& parent, bool horizontal, const GUIElementStyle* style = nullptr);
		static GUIScrollBarHandle* create(GUIWidget& parent, bool horizontal, const GUIOptions& layoutOptions, 
			const GUIElementStyle* style = nullptr);

		void setHandleSize(CM::UINT32 size);
		void setHandlePos(float pct);

		CM::UINT32 getScrollableSize() const;
		float getHandlePos() const;

		CM::UINT32 getMaxSize() const;

		virtual CM::Vector2I _getOptimalSize() const;

		boost::signal<void(float newPosition)> onHandleMoved;
	protected:
		~GUIScrollBarHandle();

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
	private:
		ImageSprite* mImageSprite;
		CM::UINT32 mHandleSize;
		HSpriteTexture mCurTexture;
		bool mHorizontal; // Otherwise its vertical
		float mHandlePos;
		CM::INT32 mDragStartPos;
		bool mMouseOverHandle;
		bool mHandleDragged;

		GUIScrollBarHandle(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		bool isOnHandle(const CM::Vector2I& pos) const;
	};
}