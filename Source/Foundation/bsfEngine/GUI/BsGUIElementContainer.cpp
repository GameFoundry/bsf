//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "GUI/BsGUIElementContainer.h"
#include "GUI/BsGUISkin.h"
#include "GUI/BsGUIManager.h"

namespace bs
{
	GUIElementContainer::GUIElementContainer(const GUIDimensions& dimensions, const String& style,
		GUIElementOptions options)
		:GUIElement(style, dimensions, options)
	{
		mOptionFlags.set(GUIElementOption::ClickThrough);
	}

	UINT32 GUIElementContainer::_getNumRenderElements() const
	{
		return 0;
	}

	const SpriteMaterialInfo& GUIElementContainer::_getMaterial(UINT32 renderElementIdx, SpriteMaterial** material) const
	{
		BS_EXCEPT(InvalidStateException, "Trying to retrieve a material from an element with no render elements.");
		static SpriteMaterialInfo dummy;
		return dummy;
	}

	void GUIElementContainer::_getMeshInfo(UINT32 renderElementIdx, UINT32& numVertices, UINT32& numIndices, GUIMeshType& type) const
	{
		numVertices = 0;
		numIndices = 0;
		type = GUIMeshType::Triangle;
	}

	void GUIElementContainer::_fillBuffer(UINT8* vertices, UINT32* indices, UINT32 vertexOffset, UINT32 indexOffset,
		UINT32 maxNumVerts, UINT32 maxNumIndices, UINT32 renderElementIdx) const
	{ }

	Vector2I GUIElementContainer::_getOptimalSize() const
	{
		return Vector2I();
	}

	void GUIElementContainer::setFocus(bool enabled, bool clear)
	{
		if(mFocusElement)
			mFocusElement->setFocus(enabled, clear);
		else
			GUIElement::setFocus(enabled, clear);
	}

	bool GUIElementContainer::_commandEvent(const GUICommandEvent& ev)
	{
		// Make sure to pass through focus events to elements below
		if (ev.getType() == GUICommandEventType::FocusGained)
			return false;
		else if (ev.getType() == GUICommandEventType::FocusLost)
			return false;

		return GUIElement::_commandEvent(ev);
	}
}
