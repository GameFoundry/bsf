#include "BsGUIHelper.h"
#include "BsSpriteTexture.h"
#include "BsGUIElementStyle.h"
#include "BsGUILayoutOptions.h"
#include "CmTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Vector2I GUIHelper::calcOptimalContentsSize(const GUIContent& content, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions)
	{
		Vector2I textContentBounds = calcOptimalContentsSize(content.getText(), style, layoutOptions);

		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;
		if(content.getImage() != nullptr)
		{
			contentWidth += content.getImage()->getTexture()->getWidth();
			contentHeight += content.getImage()->getTexture()->getHeight();
		}

		return Vector2I(std::max((UINT32)textContentBounds.x, contentWidth), std::max((UINT32)textContentBounds.y, contentHeight));
	}

	CM::Vector2I GUIHelper::calcOptimalContentsSize(const WString& text, const GUIElementStyle& style, const GUILayoutOptions& layoutOptions)
	{
		UINT32 wordWrapWidth = 0;

		if(style.wordWrap)
		{
			if(layoutOptions.fixedWidth)
				wordWrapWidth = layoutOptions.width;
			else
				wordWrapWidth = layoutOptions.maxWidth;
		}

		UINT32 contentWidth = style.margins.left + style.margins.right + style.contentOffset.left + style.contentOffset.right;
		UINT32 contentHeight = style.margins.top + style.margins.bottom + style.contentOffset.top + style.contentOffset.bottom;

		TextData textData(text, style.font, style.fontSize, wordWrapWidth, 0, style.wordWrap);

		contentWidth += textData.getWidth();
		contentHeight += textData.getHeight();

		return Vector2I(contentWidth, contentHeight);
	}
}