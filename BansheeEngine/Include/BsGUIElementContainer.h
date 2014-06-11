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
		GUIElementContainer(const GUILayoutOptions& layoutOptions);
		virtual ~GUIElementContainer();

		/**
		 * @copydoc GUIElement::getNumRenderElements
		 */
		virtual UINT32 getNumRenderElements() const;

		/**
		 * @copydoc GUIElement::getMaterial
		 */
		virtual const GUIMaterialInfo& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::getNumQuads
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @copydoc GUIElement::fillBuffer
		 */
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
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