#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarHandle : public GUIElement
	{
		enum class State
		{
			Normal, Hover, Active
		};

	public:
		static const String& getGUITypeName();

		static GUIScrollBarHandle* create(bool horizontal, const String& styleName = StringUtil::BLANK);
		static GUIScrollBarHandle* create(bool horizontal, const GUIOptions& layoutOptions, 
			const String& styleName = StringUtil::BLANK);

		void setHandleSize(UINT32 size);
		void setHandlePos(float pct);

		UINT32 getScrollableSize() const;
		float getHandlePos() const;

		UINT32 getMaxSize() const;

		virtual Vector2I _getOptimalSize() const;

		boost::signal<void(float newPosition)> onHandleMoved;
	protected:
		~GUIScrollBarHandle();

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
		ImageSprite* mImageSprite;
		UINT32 mHandleSize;
		
		bool mHorizontal; // Otherwise its vertical
		float mHandlePos;
		INT32 mDragStartPos;
		bool mMouseOverHandle;
		bool mHandleDragged;
		State mState;

		GUIScrollBarHandle(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		bool isOnHandle(const Vector2I& pos) const;

		const HSpriteTexture& getActiveTexture() const;
	};
}