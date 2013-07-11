#include "BsGUIInputSelection.h"
#include "BsImageSprite.h"
#include "BsGUIManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIInputSelection::GUIInputSelection(const TEXT_SPRITE_DESC& textDesc)
		:mSelectionStart(0), mSelectionEnd(0), mSelectionAnchor(0), mSelectionDragAnchor(0),
		mTextSprite(nullptr)
	{
		mTextSprite = cm_new<TextSprite, PoolAlloc>();

		mTextSprite->update(mTextDesc);
	}

	GUIInputSelection::~GUIInputSelection()
	{
		for(auto& sprite : mSprites)
			cm_delete<PoolAlloc>(sprite);

		cm_delete<PoolAlloc>(mTextSprite);
	}

	void GUIInputSelection::updateText(const TEXT_SPRITE_DESC& textDesc)
	{
		mTextDesc = textDesc;
		mTextDesc.clipRect = Rect(0, 0, 0, 0); // No clipping otherwise we don't know position of chars
		// outside of the element, which is something we need when moving the cursor

		mTextSprite->update(mTextDesc);
	}

	void GUIInputSelection::updateSprite(const CM::Int2& offset)
	{
		Vector<Rect>::type selectionRects = getSelectionRects();

		INT32 diff = (INT32)(mSprites.size() - selectionRects.size());

		if(diff > 0)
		{
			for(UINT32 i = (UINT32)selectionRects.size(); i < (UINT32)mSprites.size(); i++)
				cm_delete(mSprites[i]);

			mSprites.erase(mSprites.begin() + selectionRects.size(), mSprites.end());
		}
		else if(diff < 0)
		{
			for(INT32 i = diff; i < 0; i++)
			{
				ImageSprite* newSprite = cm_new<ImageSprite>();
				mSprites.push_back(newSprite);
			}
		}

		UINT32 idx = 0;
		for(auto& sprite : mSprites)
		{
			IMAGE_SPRITE_DESC desc;
			desc.offset = Int2(selectionRects[idx].x, selectionRects[idx].y);
			desc.width = selectionRects[idx].width;
			desc.height = selectionRects[idx].height;
			desc.clipRect = Rect(mTextDesc.offset.x - selectionRects[idx].x, 
				mTextDesc.offset.y - selectionRects[idx].y, mTextDesc.width, mTextDesc.height);
			desc.texture = GUIManager::instance().getTextSelectionTexture();

			sprite->update(desc);
			idx++;
		}
	}

	Vector<Rect>::type GUIInputSelection::getSelectionRects() const
	{
		Vector<Rect>::type selectionRects;

		if(mSelectionStart == mSelectionEnd)
			return selectionRects;

		UINT32 startLine = mTextSprite->getLineForChar(mSelectionStart);

		UINT32 endLine = startLine;
		if(mSelectionEnd > 0)
			endLine = mTextSprite->getLineForChar(mSelectionEnd - 1, true);

		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(startLine);

			UINT32 startCharIdx = mSelectionStart;

			UINT32 endCharIdx = mSelectionEnd - 1;
			if(startLine != endLine)
			{
				endCharIdx = lineDesc.getEndChar(false);
				if(endCharIdx > 0)
					endCharIdx = endCharIdx - 1;
			}

			if(!isNewlineChar(startCharIdx) && !isNewlineChar(endCharIdx))
			{
				Rect startChar = mTextSprite->getCharRect(startCharIdx);
				Rect endChar = mTextSprite->getCharRect(endCharIdx);

				Rect selectionRect;
				selectionRect.x = startChar.x;
				selectionRect.y = lineDesc.getLineYStart();
				selectionRect.height = lineDesc.getLineHeight();
				selectionRect.width = (endChar.x + endChar.width) - startChar.x;

				selectionRects.push_back(selectionRect);
			}
		}

		for(UINT32 i = startLine + 1; i < endLine; i++)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(i);
			if(lineDesc.getStartChar() == lineDesc.getEndChar() || isNewlineChar(lineDesc.getStartChar()))
				continue;

			UINT32 endCharIdx = lineDesc.getEndChar(false);
			if(endCharIdx > 0)
				endCharIdx = endCharIdx - 1;

			Rect startChar = mTextSprite->getCharRect(lineDesc.getStartChar());
			Rect endChar = mTextSprite->getCharRect(endCharIdx);

			Rect selectionRect;
			selectionRect.x = startChar.x;
			selectionRect.y = lineDesc.getLineYStart();
			selectionRect.height = lineDesc.getLineHeight();
			selectionRect.width = (endChar.x + endChar.width) - startChar.x;

			selectionRects.push_back(selectionRect);
		}

		if(startLine != endLine)
		{
			const SpriteLineDesc& lineDesc = mTextSprite->getLineDesc(endLine);

			if(lineDesc.getStartChar() != lineDesc.getEndChar() && !isNewlineChar(lineDesc.getStartChar()))
			{
				UINT32 endCharIdx = mSelectionEnd - 1;

				if(!isNewlineChar(endCharIdx))
				{
					Rect startChar = mTextSprite->getCharRect(lineDesc.getStartChar());
					Rect endChar = mTextSprite->getCharRect(endCharIdx);

					Rect selectionRect;
					selectionRect.x = startChar.x;
					selectionRect.y = lineDesc.getLineYStart();
					selectionRect.height = lineDesc.getLineHeight();
					selectionRect.width = (endChar.x + endChar.width) - startChar.x;

					selectionRects.push_back(selectionRect);
				}
			}
		}

		return selectionRects;
	}

	bool GUIInputSelection::isNewlineChar(CM::UINT32 charIdx) const
	{
		if(mTextDesc.text[charIdx] == '\n')
			return true;

		return false;
	}
}