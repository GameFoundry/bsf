#include "BsGUILabel.h"
#include "BsGUIElementStyle.h"
#include "BsTextSprite.h"
#include "BsGUISkin.h"
#include "BsGUIWidget.h"
#include "BsGUIDimensions.h"
#include "BsGUIHelper.h"
#include "BsTextData.h"

namespace BansheeEngine
{
	GUILabel::GUILabel(const String& styleName, const GUIContent& content, const GUIDimensions& dimensions)
		:GUIElement(styleName, dimensions), mContent(content)
	{
		mTextSprite = bs_new<TextSprite, PoolAlloc>();

		mLocStringUpdatedConn = mContent.getText().addOnStringModifiedCallback(std::bind(&GUILabel::markContentAsDirty, this));
	}

	GUILabel::~GUILabel()
	{
		mLocStringUpdatedConn.disconnect();
		bs_delete<PoolAlloc>(mTextSprite);
	}

	UINT32 GUILabel::_getNumRenderElements() const
	{
		return mTextSprite->getNumRenderElements();
	}

	const GUIMaterialInfo& GUILabel::_getMaterial(UINT32 renderElementIdx) const
	{
		return mTextSprite->getMaterial(renderElementIdx);
	}

	UINT32 GUILabel::_getNumQuads(UINT32 renderElementIdx) const
	{
		return mTextSprite->getNumQuads(renderElementIdx);
	}

	void GUILabel::updateRenderElementsInternal()
	{		
		mDesc.font = _getStyle()->font;
		mDesc.fontSize = _getStyle()->fontSize;
		mDesc.wordWrap = _getStyle()->wordWrap;
		mDesc.horzAlign = _getStyle()->textHorzAlign;
		mDesc.vertAlign = _getStyle()->textVertAlign;
		mDesc.width = mWidth;
		mDesc.height = mHeight;
		mDesc.text = mContent.getText();
		mDesc.color = mColor * _getStyle()->normal.textColor;;

		mTextSprite->update(mDesc, (UINT64)_getParentWidget());

		GUIElement::updateRenderElementsInternal();
	}

	void GUILabel::updateClippedBounds()
	{
		mClippedBounds = mTextSprite->getBounds(mOffset, mClipRect);
	}

	Vector2I GUILabel::_getOptimalSize() const
	{
		return GUIHelper::calcOptimalContentsSize(mContent, *_getStyle(), _getDimensions());
	}

	void GUILabel::_fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		mTextSprite->fillBuffer(vertices, uv, indices, startingQuad, maxNumQuads, vertexStride, indexStride, renderElementIdx, mOffset, mClipRect);
	}

	void GUILabel::setContent(const GUIContent& content)
	{
		mLocStringUpdatedConn.disconnect();
		mLocStringUpdatedConn = content.getText().addOnStringModifiedCallback(std::bind(&GUILabel::markContentAsDirty, this));

		mContent = content;
		
		markContentAsDirty();
	}

	void GUILabel::setTint(const Color& color)
	{
		mColor = color;

		markContentAsDirty();
	}

	GUILabel* GUILabel::create(const HString& text, const String& styleName)
	{
		return create(GUIContent(text), styleName);
	}

	GUILabel* GUILabel::create(const HString& text, const GUIOptions& options, const String& styleName)
	{
		return create(GUIContent(text), options, styleName);
	}

	GUILabel* GUILabel::create(const GUIContent& content, const String& styleName)
	{
		return new (bs_alloc<GUILabel, PoolAlloc>()) GUILabel(getStyleName<GUILabel>(styleName), content, GUIDimensions::create());
	}

	GUILabel* GUILabel::create(const GUIContent& content, const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUILabel, PoolAlloc>()) GUILabel(getStyleName<GUILabel>(styleName), content, GUIDimensions::create(options));
	}

	const String& GUILabel::getGUITypeName()
	{
		static String typeName = "Label";
		return typeName;
	}
}