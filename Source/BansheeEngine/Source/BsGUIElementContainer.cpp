//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGUIElementContainer.h"
#include "BsGUISkin.h"
#include "BsGUIManager.h"

namespace BansheeEngine
{
	GUIElementContainer::GUIElementContainer(const GUIDimensions& dimensions, const String& style)
		:GUIElement(style, dimensions)
	{ }

	GUIElementContainer::~GUIElementContainer()
	{ }

	UINT32 GUIElementContainer::_getNumRenderElements() const
	{
		return 0;
	}

	const SpriteMaterialInfo& GUIElementContainer::_getMaterial(UINT32 renderElementIdx) const
	{
		BS_EXCEPT(InvalidStateException, "Trying to retrieve a material from an element with no render elements.");
		static SpriteMaterialInfo dummy;
		return dummy;
	}

	void GUIElementContainer::_getMeshSize(UINT32 renderElementIdx, UINT32& numVertices, UINT32& numIndices) const
	{
		numVertices = 0;
		numIndices = 0;
	}

	void GUIElementContainer::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 vertexOffset, UINT32 indexOffset,
		UINT32 maxNumVerts, UINT32 maxNumIndices, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	Vector2I GUIElementContainer::_getOptimalSize() const
	{
		return Vector2I();
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