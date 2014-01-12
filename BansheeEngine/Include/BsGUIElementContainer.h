#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIElementContainer : public BS::GUIElement
	{
	protected:
		GUIElementContainer(BS::GUIWidget& parent, const BS::GUILayoutOptions& layoutOptions);
		virtual ~GUIElementContainer();

		/**
		 * @copydoc GUIElement::getNumRenderElements()
		 */
		virtual CM::UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial()
		 */
		virtual const BS::GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

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
		 * @copydoc GUIElement::updateBounds()
		 */
		virtual void updateClippedBounds();

		virtual CM::Vector2I _getOptimalSize() const;
		void _changeParentWidget(BS::GUIWidget* widget);

		virtual CM::UINT32 getNumChildElements() const = 0;
		virtual GUIElement* getChildElement(CM::UINT32 idx) const = 0;
	};
}