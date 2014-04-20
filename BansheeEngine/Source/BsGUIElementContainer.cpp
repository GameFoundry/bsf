#include "BsGUIElementContainer.h"
#include "BsGUISkin.h"

namespace BansheeEngine
{
	GUIElementContainer::GUIElementContainer(const GUILayoutOptions& layoutOptions)
		:GUIElement(StringUtil::BLANK, layoutOptions)
	{ }

	GUIElementContainer::~GUIElementContainer()
	{ }

	UINT32 GUIElementContainer::getNumRenderElements() const
	{
		return 0;
	}

	const GUIMaterialInfo& GUIElementContainer::getMaterial(UINT32 renderElementIdx) const
	{
		CM_EXCEPT(InvalidStateException, "Trying to retrieve a material from an element with no render elements.");
	}

	UINT32 GUIElementContainer::getNumQuads(UINT32 renderElementIdx) const
	{
		return 0;
	}

	void GUIElementContainer::updateClippedBounds()
	{
		mClippedBounds = RectI(0, 0, 0, 0); // We don't want any mouse input for this element. This is just a container.
	}

	void GUIElementContainer::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{ }

	Vector2I GUIElementContainer::_getOptimalSize() const
	{
		return Vector2I();
	}
}