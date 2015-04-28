#include "BsGUIHelper.h"
#include "BsSpriteTexture.h"
#include "BsGUIElementStyle.h"
#include "BsGUIDimensions.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	Vector2I GUIHelper::calcOptimalContentsSize(const Vector2I& contentSize, const GUIElementStyle& style, const GUIDimensions& layoutOptions)
	{
		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;

		return Vector2I(std::max((UINT32)contentSize.x, contentWidth), std::max((UINT32)contentSize.y, contentHeight));
	}

	Vector2I GUIHelper::calcOptimalContentsSize(const GUIContent& content, const GUIElementStyle& style, const GUIDimensions& layoutOptions)
	{
		Vector2I textContentBounds = calcOptimalContentsSize(content.getText(), style, layoutOptions);

		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;
		if(content.getImage() != nullptr)
		{
			contentWidth += content.getImage()->getWidth();
			contentHeight += content.getImage()->getHeight();
		}

		return Vector2I(std::max((UINT32)textContentBounds.x, contentWidth), std::max((UINT32)textContentBounds.y, contentHeight));
	}

	Vector2I GUIHelper::calcOptimalContentsSize(const WString& text, const GUIElementStyle& style, const GUIDimensions& layoutOptions)
	{
		UINT32 wordWrapWidth = 0;

		if(style.wordWrap)
			wordWrapWidth = layoutOptions.maxWidth;

		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;

		if(style.font != nullptr)
		{
			TextData textData(text, style.font, style.fontSize, wordWrapWidth, 0, style.wordWrap);

			contentWidth += textData.getWidth();
			contentHeight += textData.getNumLines() * textData.getLineHeight(); 
		}

		return Vector2I(contentWidth, contentHeight);
	}
}