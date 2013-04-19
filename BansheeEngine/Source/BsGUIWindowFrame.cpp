#include "BsGUIWindowFrame.h"
#include "BsImageSprite.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsSpriteTexture.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	const String& GUIWindowFrame::getGUITypeName()
	{
		static String name = "WindowFrame";
		return name;
	}

	GUIWindowFrame::GUIWindowFrame(GUIWidget* parent, const SpriteTexturePtr& texture)
		:GUIElement(parent), mTexture(texture)
	{
		const GUISkin* skin = parent->getGUISkin();

		mStyle = skin->getStyle(getGUITypeName());
		mImageSprite = CM_NEW(ImageSprite, PoolAlloc) ImageSprite();

		IMAGE_SPRITE_DESC desc;
		desc.texture = texture;
		desc.width = texture->getTexture()->getWidth();
		desc.height = texture->getTexture()->getHeight();
		mImageSprite->update(desc);

		mBounds = mImageSprite->getBounds();
	}

	GUIWindowFrame::~GUIWindowFrame()
	{
		CM_DELETE(mImageSprite, ImageSprite, PoolAlloc);
	}

	GUIWindowFrame* GUIWindowFrame::create(GUIWidget* parent, const SpriteTexturePtr& texture)
	{
		return CM_NEW(GUIWindowFrame, PoolAlloc) GUIWindowFrame(parent, texture);
	}

	UINT32 GUIWindowFrame::getNumRenderElements() const
	{
		return mImageSprite->getNumRenderElements();
	}

	const HMaterial& GUIWindowFrame::getMaterial(UINT32 renderElementIdx) const
	{
		return mImageSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUIWindowFrame::getNumQuads(UINT32 renderElementIdx) const
	{
		return mImageSprite->getNumQuads(renderElementIdx);
	}

	void GUIWindowFrame::fillBuffer(CM::Vector2* vertices, CM::Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const
	{
		mImageSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, renderElementIdx);
	}
}