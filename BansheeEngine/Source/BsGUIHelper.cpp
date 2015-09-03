#include "BsGUIHelper.h"
#include "BsSpriteTexture.h"
#include "BsGUIElementStyle.h"
#include "BsGUIDimensions.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	Vector2I GUIHelper::calcOptimalContentsSize(const Vector2I& contentSize, const GUIElementStyle& style, const GUIDimensions& dimensions)
	{
		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;

		return Vector2I(std::max((UINT32)contentSize.x, contentWidth), std::max((UINT32)contentSize.y, contentHeight));
	}

	Vector2I GUIHelper::calcOptimalContentsSize(const GUIContent& content, const GUIElementStyle& style, const GUIDimensions& dimensions)
	{
		Vector2I textContentBounds = calcOptimalContentsSize((const WString&)content.getText(), style, dimensions);

		Vector2I imageSize;
		imageSize.x = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		imageSize.y = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;
		if(content.getImage() != nullptr)
		{
			imageSize.x += content.getImage()->getWidth();
			imageSize.y += content.getImage()->getHeight();

			imageSize = dimensions.calculateSizeRange(imageSize).optimal;
		}

		return Vector2I(std::max((UINT32)textContentBounds.x, (UINT32)imageSize.x), std::max((UINT32)textContentBounds.y, (UINT32)imageSize.y));
	}

	Vector2I GUIHelper::calcOptimalContentsSize(const WString& text, const GUIElementStyle& style, const GUIDimensions& dimensions)
	{
		UINT32 wordWrapWidth = 0;

		if(style.wordWrap)
			wordWrapWidth = dimensions.maxWidth;

		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;

		if(style.font != nullptr)
		{
			bs_frame_mark();

			TextData<FrameAlloc> textData(text, style.font, style.fontSize, wordWrapWidth, 0, style.wordWrap);

			contentWidth += textData.getWidth();
			contentHeight += textData.getNumLines() * textData.getLineHeight(); 

			bs_frame_clear();
		}

		return Vector2I(contentWidth, contentHeight);
	}
}