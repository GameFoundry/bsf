#include "BsGUIElementContainer.h"
#include "BsGUISkin.h"

namespace BansheeEngine
{
	GUIElementContainer::GUIElementContainer(const GUIDimensions& layoutOptions, const String& style)
		:GUIElement(style, layoutOptions)
	{ }

	GUIElementContainer::~GUIElementContainer()
	{ }

	void GUIElementContainer::_setPosition(const Vector2I& offset)
	{
		if (mOffset != offset)
			markContentAsDirty();

		GUIElement::_setPosition(offset);
	}

	UINT32 GUIElementContainer::_getNumRenderElements() const
	{
		return 0;
	}

	const GUIMaterialInfo& GUIElementContainer::_getMaterial(UINT32 renderElementIdx) const
	{
		BS_EXCEPT(InvalidStateException, "Trying to retrieve a material from an element with no render elements.");
	}

	UINT32 GUIElementContainer::_getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIElementContainer::updateClippedBounds()
	{
		mClippedBounds = Rect2I(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	void GUIElementContainer::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	Vector2I GUIElementContainer::_getOptimalSize() const
	{
		return Vector2I();
	}
}