#include "BsGUILayoutX.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "CmMath.h"
#include "CmVector2I.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GUILayoutX::_updateOptimalLayoutSizes()
	{
		// Update all children first, otherwise we can't determine out own optimal size
		GUIElementBase::_updateOptimalLayoutSizes();

		if(mChildren.size() != mOptimalSizes.size())
			mOptimalSizes.resize(mChildren.size());

		mOptimalWidth = 0;
		mOptimalHeight = 0;

		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			UINT32 optimalWidth = 0;
			UINT32 optimalHeight = 0;

			if(child->_getType() == GUIElementBase::Type::FixedSpace)
			{
				GUIFixedSpace* space = static_cast<GUIFixedSpace*>(child);
				optimalWidth = space->getSize();
			}
			else if(child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

				Vector2I optimalSize;
				if(!layoutOptions.fixedWidth || !layoutOptions.fixedHeight)
					optimalSize = child->_getOptimalSize();

				if(layoutOptions.fixedWidth)
				{
					optimalWidth = layoutOptions.width;
				}
				else
				{
					optimalWidth = optimalSize.x;

					if(layoutOptions.minWidth > 0)
						optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

					if(layoutOptions.maxWidth > 0)
						optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);
				}

				if(layoutOptions.fixedHeight)
					optimalHeight = layoutOptions.height;
				else
				{
					optimalHeight = optimalSize.y;

					if(layoutOptions.minHeight > 0)
						optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

					if(layoutOptions.maxHeight > 0)
						optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);
				}
			}
			else if(child->_getType() == GUIElementBase::Type::Layout)
			{
				optimalWidth = child->_getOptimalSize().x;
			}

			mOptimalSizes[childIdx].x = optimalWidth;
			mOptimalWidth += optimalWidth;

			mOptimalSizes[childIdx].y = optimalHeight;
			mOptimalHeight = std::max(mOptimalHeight, optimalHeight);

			childIdx++;
		}
	}

	void GUILayoutX::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 totalOptimalSize = _getOptimalSize().x;
		UINT32 totalNonClampedSize = 0;
		UINT32 numNonClampedElements = 0;
		UINT32 numFlexibleSpaces = 0;

		bool* processedElements = nullptr;
		UINT32* elementSizes = nullptr;
		float* elementScaleWeights = nullptr;

		if(mChildren.size() > 0)
		{
			processedElements = stackAllocN<bool>((UINT32)mChildren.size());
			memset(processedElements, 0, mChildren.size() * sizeof(bool));

			elementSizes = stackAllocN<UINT32>((UINT32)mChildren.size());
			memset(elementSizes, 0, mChildren.size() * sizeof(UINT32));

			elementScaleWeights = stackAllocN<float>((UINT32)mChildren.size());
			memset(elementScaleWeights, 0, mChildren.size() * sizeof(float));
		}

		// Set initial sizes, count number of children per type and mark fixed elements as already processed
		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			elementSizes[childIdx] = mOptimalSizes[childIdx].x;

			if(child->_getType() == GUIElementBase::Type::FixedSpace)
			{
				processedElements[childIdx] = true;
			}
			else if(child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
					processedElements[childIdx] = true;
				else
				{
					numNonClampedElements++;
					totalNonClampedSize += elementSizes[childIdx];
				}
			}
			else if(child->_getType() == GUIElementBase::Type::Layout)
			{
				numNonClampedElements++;
				totalNonClampedSize += elementSizes[childIdx];
			}
			else if(child->_getType() == GUIElementBase::Type::FlexibleSpace)
			{
				numFlexibleSpaces++;
				numNonClampedElements++;
			}

			childIdx++;
		}

		// If there is some room left, calculate flexible space sizes (since they will fill up all that extra room)
		if(width > totalOptimalSize)
		{
			UINT32 extraSize = width - totalOptimalSize;
			UINT32 remainingSize = extraSize;

			// Flexible spaces always expand to fill up all unused space
			if(numFlexibleSpaces > 0)
			{
				float avgSize = remainingSize / (float)numFlexibleSpaces;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					UINT32 extraWidth = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementWidth = elementSizes[childIdx] + extraWidth;

					// Clamp if needed
					if(child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						processedElements[childIdx] = true;
						numNonClampedElements--;
						elementSizes[childIdx] = elementWidth;

						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}

					childIdx++;
				}

				totalOptimalSize = width;
			}
		}

		// Determine weight scale for every element. When scaling elements up/down they will be scaled based on this weight.
		// Weight is to ensure all elements are scaled fairly, so elements that are large will get effected more than smaller elements.
		childIdx = 0;
		float invOptimalSize = 1.0f / totalNonClampedSize;
		for(auto& child : mChildren)
		{
			if(processedElements[childIdx])
			{
				childIdx++;
				continue;
			}

			elementScaleWeights[childIdx] = invOptimalSize * elementSizes[childIdx];

			childIdx++;
		}

		// Our optimal size is larger than maximum allowed, so we need to reduce size of some elements
		if(totalOptimalSize > width)
		{
			UINT32 extraSize = totalOptimalSize - width;
			UINT32 remainingSize = extraSize;

			// Iterate until we reduce everything so it fits, while maintaining
			// equal average sizes using the weights we calculated earlier
			while(remainingSize > 0 && numNonClampedElements > 0)
			{
				UINT32 totalRemainingSize = remainingSize;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					float avgSize = totalRemainingSize * elementScaleWeights[childIdx];
					
					UINT32 extraWidth = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementWidth = (UINT32)std::max(0, (INT32)elementSizes[childIdx] - (INT32)extraWidth);

					// Clamp if needed
					if(child->_getType() == GUIElementBase::Type::Element)
					{
						GUIElement* element = static_cast<GUIElement*>(child);
						const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

						if(elementWidth == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}
						else if(layoutOptions.minWidth > 0 && elementWidth < layoutOptions.minWidth)
						{
							elementWidth = layoutOptions.minWidth;
							
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraWidth = elementSizes[childIdx] - elementWidth;
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child->_getType() == GUIElementBase::Type::Layout)
					{
						if(elementWidth == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraWidth = elementSizes[childIdx] - elementWidth;
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						elementSizes[childIdx] = 0;
						processedElements[childIdx] = true;
						numNonClampedElements--;
					}

					childIdx++;
				}
			}
		}
		else // We are smaller than the allowed maximum, so try to expand some elements
		{
			UINT32 extraSize = width - totalOptimalSize;
			UINT32 remainingSize = extraSize;

			// Iterate until we reduce everything so it fits, while maintaining
			// equal average sizes using the weights we calculated earlier
			while(remainingSize > 0 && numNonClampedElements > 0)
			{
				UINT32 totalRemainingSize = remainingSize;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					float avgSize = totalRemainingSize * elementScaleWeights[childIdx];
					UINT32 extraWidth = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementWidth = elementSizes[childIdx] + extraWidth;

					// Clamp if needed
					if(child->_getType() == GUIElementBase::Type::Element)
					{
						GUIElement* element = static_cast<GUIElement*>(child);
						const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

						if(elementWidth == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}
						else if(layoutOptions.maxWidth > 0 && elementWidth > layoutOptions.maxWidth)
						{
							elementWidth = layoutOptions.maxWidth;

							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraWidth = elementWidth - elementSizes[childIdx];
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child->_getType() == GUIElementBase::Type::Layout)
					{
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						processedElements[childIdx] = true;
						numNonClampedElements--;
					}

					childIdx++;
				}
			}
		}

		// Now that we have all the sizes, actually assign them
		// Also assign offsets, clip rectangles and depth
		UINT32 xOffset = 0;
		childIdx = 0;

		mActualWidth = 0;
		mActualHeight = 0;
		for(auto& child : mChildren)
		{
			UINT32 elemWidth = elementSizes[childIdx];

			if(child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				element->_setWidth(elemWidth);

				UINT32 elemHeight = mOptimalSizes[childIdx].y;
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();
				if(!layoutOptions.fixedHeight)
				{
					elemHeight = height;
					if(layoutOptions.minHeight > 0 && elemHeight < layoutOptions.minHeight)
						elemHeight = layoutOptions.minHeight;

					if(layoutOptions.maxHeight > 0 && elemHeight > layoutOptions.maxHeight)
						elemHeight = layoutOptions.maxHeight;
				}

				element->_setHeight(elemHeight);

				INT32 yOffset = Math::ceilToInt(((INT32)height - (INT32)element->_getHeight()) * 0.5f);
				yOffset = std::max(0, yOffset);

				Vector2I offset(x + xOffset, y + yOffset);
				element->_setOffset(offset);
				element->_setWidgetDepth(widgetDepth);
				element->_setAreaDepth(areaDepth);

				RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
				element->_setClipRect(elemClipRect);

				RectI newClipRect(offset.x, offset.y, elemWidth, elemHeight);
				element->_updateLayoutInternal(offset.x, offset.y, elemWidth, elemHeight, newClipRect, widgetDepth, areaDepth);

				mActualHeight = std::max(mActualHeight, elemHeight);
			}
			else if(child->_getType() == GUIElementBase::Type::Layout)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				RectI newClipRect(x + xOffset, y, elemWidth, height);
				newClipRect.clip(clipRect);
				layout->_updateLayoutInternal(x + xOffset, y, elemWidth, height, newClipRect, widgetDepth, areaDepth);

				UINT32 childHeight = layout->_getActualHeight();
				mActualHeight = std::max(mActualHeight, childHeight);

				// It's possible all elements didn't fit in the child layout size we provided, in which case adjust our measurements
				elemWidth = layout->_getActualWidth();
			}

			mActualWidth += elemWidth;
			xOffset += elemWidth;
			childIdx++;
		}

		if(elementScaleWeights != nullptr)
			stackDeallocLast(elementScaleWeights);

		if(elementSizes != nullptr)
			stackDeallocLast(elementSizes);

		if(processedElements != nullptr)
			stackDeallocLast(processedElements);

		_markAsClean();
	}
}