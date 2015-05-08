#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper GUI element class that is meant for hold other GUI elements.
	 *			(Similar as GUILayout, but you may position elements manually).
	 */
	class BS_EXPORT GUIElementContainer : public GUIElement
	{
	protected:
		GUIElementContainer(const GUIDimensions& dimensions, const String& style = StringUtil::BLANK);
		virtual ~GUIElementContainer();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 _getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::updateClippedBounds
		 */
		virtual void updateClippedBounds();

		/**
		 * @copydoc GUIElement::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;
	};
}