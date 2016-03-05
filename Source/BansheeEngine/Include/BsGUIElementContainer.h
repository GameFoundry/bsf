//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElement.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup GUI
	 *  @{
	 */

	/** Base for GUI elements that combine multiple GUI elements into one more complex element. */
	class BS_EXPORT GUIElementContainer : public GUIElement
	{
	protected:
		GUIElementContainer(const GUIDimensions& dimensions, const String& style = StringUtil::BLANK);
		virtual ~GUIElementContainer();

		/** @copydoc GUIElement::_getNumRenderElements */
		virtual UINT32 _getNumRenderElements() const override;

		/** @copydoc GUIElement::_getMaterial */
		virtual const SpriteMaterialInfo& _getMaterial(UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_getNumQuads */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_fillBuffer */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const override;

		/** @copydoc GUIElement::_getOptimalSize */
		virtual Vector2I _getOptimalSize() const override;
	};

	/** @} */
	/** @endcond */
}