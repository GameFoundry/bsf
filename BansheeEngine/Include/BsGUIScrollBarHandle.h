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
		static GUIScrollBarHandle* create(GUIWidget& parent, bool horizontal, const GUILayoutOptions& layoutOptions, 
			const GUIElementStyle* style = nullptr);

		void setHandleSize(CM::UINT32 size);
		void setHandlePos(float pct);
		void setPageSize(CM::UINT32 size);

		CM::UINT32 getScrollableSize() const;
		float getHandlePos() const;

		CM::UINT32 getMaxSize() const;

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

		virtual CM::UINT32 _getOptimalWidth() const;
		virtual CM::UINT32 _getOptimalHeight() const;
	private:
		ImageSprite* mImageSprite;
		CM::UINT32 mHandleSize;
		SpriteTexturePtr mCurTexture;
		bool mHorizontal; // Otherwise its vertical
		CM::INT32 mHandlePos;
		CM::INT32 mDragStartPos;
		bool mMouseOverHandle;
		bool mHandleDragged;
		CM::UINT32 mPageSize;

		static const CM::UINT32 DefaultPageScrollAmount;

		GUIScrollBarHandle(GUIWidget& parent, bool horizontal, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		bool isOnHandle(const CM::Int2& pos) const;
	};
}