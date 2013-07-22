#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarVert : public GUIElement
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIScrollBarVert* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIScrollBarVert* create(GUIWidget& parent, const GUILayoutOptions& layoutOptions, const GUIElementStyle* style = nullptr);

		boost::signal<void(float newPosition)> scrollPositionChanged;

		void setHandleSize(CM::UINT32 size);
		void setScrollPos(float pct);

		CM::UINT32 getMaxHandleSize() const;
	protected:
		~GUIScrollBarVert();

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

		virtual CM::UINT32 _getRenderElementDepth(CM::UINT32 renderElementIdx) const;
	private:
		GUIScrollBarVert(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);

		GUILayout* mLayout;
		ImageSprite* mImageSprite;

		GUIButton* mUpBtn;
		GUIButton* mDownBtn;
		GUIScrollBarHandle* mHandleBtn;

		void handleMoved(float handlePct);
	};
}