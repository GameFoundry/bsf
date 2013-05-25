#include "BsGUILayoutX.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "CmMath.h"
#include "CmInt2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GUILayoutX::updateOptimalSizes()
	{
		// Update all children first, otherwise we can't determine out own optimal size
		for(auto& child : mChildren)
		{
			if(child.isLayout())
				updateOptimalSizes();
		}

		if(mChildren.size() != mOptimalSizes.size())
			mOptimalSizes.resize(mChildren.size());

		mOptimalWidth = 0;
		mOptimalHeight = 0;

		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			UINT32 optimalWidth = 0;
			UINT32 optimalHeight = 0;

			if(child.isFixedSpace())
			{
				optimalWidth = child.space->getSize();
			}
			else if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
				{
					optimalWidth = layoutOptions.width;
				}
				else
				{
					optimalWidth = child.element->_getOptimalWidth();

					if(layoutOptions.minWidth > 0)
						optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

					if(layoutOptions.maxWidth > 0)
						optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);
				}

				if(layoutOptions.fixedHeight)
					optimalHeight = layoutOptions.height;
				else
				{
					optimalHeight = child.element->_getOptimalHeight();

					if(layoutOptions.minHeight > 0)
						optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

					if(layoutOptions.maxHeight > 0)
						optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);
				}
			}
			else if(child.isLayout())
			{
				optimalWidth = child.layout->_getOptimalWidth();
			}

			mOptimalSizes[childIdx].x = optimalWidth;
			mOptimalWidth += optimalWidth;

			mOptimalSizes[childIdx].y = optimalHeight;
			mOptimalHeight = std::max(mOptimalHeight, optimalHeight);

			childIdx++;
		}
	}

	void GUILayoutX::updateInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 totalOptimalSize = _getOptimalWidth();
		UINT32 totalNonClampedSize = 0;
		UINT32 numNonClampedElements = 0;
		UINT32 numFlexibleSpaces = 0;

		bool* processedElements = stackAllocN<bool>((UINT32)mChildren.size(), HID_Main);
		memset(processedElements, 0, mChildren.size() * sizeof(bool));

		UINT32* elementSizes = stackAllocN<UINT32>((UINT32)mChildren.size(), HID_Main);
		memset(elementSizes, 0, mChildren.size() * sizeof(UINT32));

		float* elementScaleWeights = stackAllocN<float>((UINT32)mChildren.size(), HID_Main);
		memset(elementScaleWeights, 0, mChildren.size() * sizeof(float));

		// Set initial sizes, count number of children per type and mark fixed elements as already processed
		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			elementSizes[childIdx] = mOptimalSizes[childIdx].x;

			if(child.isFixedSpace())
			{
				processedElements[childIdx] = true;
			}
			else if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
					processedElements[childIdx] = true;
				else
				{
					numNonClampedElements++;
					totalNonClampedSize += elementSizes[childIdx];
				}
			}
			else if(child.isLayout())
			{
				numNonClampedElements++;
				totalNonClampedSize += elementSizes[childIdx];
			}
			else if(child.isFlexibleSpace())
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

					UINT32 extraWidth = std::min((UINT32)Math::CeilToInt(avgSize), remainingSize);
					UINT32 elementWidth = elementSizes[childIdx] + extraWidth;

					// Clamp if needed
					if(child.isFlexibleSpace())
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
					
					UINT32 extraWidth = std::min((UINT32)Math::CeilToInt(avgSize), remainingSize);
					UINT32 elementWidth = (UINT32)std::max(0, (INT32)elementSizes[childIdx] - (INT32)extraWidth);

					// Clamp if needed
					if(child.isElement())
					{
						const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

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
					else if(child.isLayout())
					{
						if(elementWidth == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraWidth = elementWidth - elementSizes[childIdx];
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child.isFlexibleSpace())
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
					UINT32 extraWidth = std::min((UINT32)Math::CeilToInt(avgSize), remainingSize);
					UINT32 elementWidth = elementSizes[childIdx] + extraWidth;

					// Clamp if needed
					if(child.isElement())
					{
						const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

						if(layoutOptions.maxWidth > 0 && elementWidth > layoutOptions.maxWidth)
						{
							extraWidth = elementSizes[childIdx] - layoutOptions.maxWidth;
							elementWidth = layoutOptions.maxWidth;

							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child.isLayout())
					{
						elementSizes[childIdx] = elementWidth;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}
					else if(child.isFlexibleSpace())
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
		for(auto& child : mChildren)
		{
			UINT32 elementWidth = elementSizes[childIdx];

			if(child.isElement())
			{
				child.element->_setWidth(elementWidth);
				child.element->_setHeight(mOptimalSizes[childIdx].y);

				UINT32 yOffset = (UINT32)Math::CeilToInt((height - child.element->_getHeight()) * 0.5f);

				Int2 offset(x + xOffset, y + yOffset);
				child.element->_setOffset(offset);
				child.element->_setWidgetDepth(widgetDepth);
				child.element->_setAreaDepth(areaDepth);

				UINT32 clippedWidth = (UINT32)std::min((INT32)child.element->_getWidth(), (INT32)width - offset.x);
				UINT32 clippedHeight = (UINT32)std::min((INT32)child.element->_getHeight(), (INT32)height - offset.y);

				child.element->_setClipRect(Rect(0, 0, clippedWidth, clippedHeight));
			}
			else if(child.isLayout())
			{
				child.layout->_update(x + xOffset, y, elementWidth, height, widgetDepth, areaDepth);
			}

			xOffset += elementWidth;
			childIdx++;
		}

		stackDeallocLast(elementScaleWeights, HID_Main);
		stackDeallocLast(elementSizes, HID_Main);
		stackDeallocLast(processedElements, HID_Main);
	}
}