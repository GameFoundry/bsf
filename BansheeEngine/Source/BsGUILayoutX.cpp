#include "BsGUILayoutX.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "CmMath.h"
#include "CmInt2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void GUILayoutX::updateInternal(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth)
	{
		UINT32 totalOptimalSize = 0;
		UINT32 numNonClampedElements = 0;
		UINT32 numFlexibleSpaces = 0;
		UINT32 numLayouts = 0;

		bool* processedElements = CM_NEW_ARRAY(bool, (UINT32)mChildren.size(), ScratchAlloc);
		memset(processedElements, 0, mChildren.size() * sizeof(bool));

		UINT32* elementSizes = CM_NEW_ARRAY(UINT32, (UINT32)mChildren.size(), ScratchAlloc);
		memset(elementSizes, 0, mChildren.size() * sizeof(UINT32));

		float* elementScaleWeights = CM_NEW_ARRAY(float, (UINT32)mChildren.size(), ScratchAlloc);
		memset(elementScaleWeights, 0, mChildren.size() * sizeof(float));

		// Set fixed-size elements and determine optimal size
		UINT32 childIdx = 0;
		for(auto& child : mChildren)
		{
			if(child.isFixedSpace())
			{
				elementSizes[childIdx] = child.space->getSize();
				totalOptimalSize += child.space->getSize();
				processedElements[childIdx] = true;
			}
			else if(child.isElement())
			{
				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

				if(layoutOptions.fixedWidth)
				{
					totalOptimalSize += layoutOptions.width;
					elementSizes[childIdx] = layoutOptions.width;
					processedElements[childIdx] = true;
				}
				else
				{
					UINT32 optimalWidth = child.element->_getOptimalWidth();

					if(layoutOptions.minWidth > 0)
						optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

					if(layoutOptions.maxWidth > 0)
						optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);

					elementSizes[childIdx] = optimalWidth;
					totalOptimalSize += optimalWidth;
					numNonClampedElements++;
				}
			}
			else if(child.isLayout())
			{
				numLayouts++;
				numNonClampedElements++;
			}
			else if(child.isFlexibleSpace())
			{
				numFlexibleSpaces++;
				numNonClampedElements++;
			}

			childIdx++;
		}

		// Determine layout size. We could just calculate optimal size of all elements in the layout
		// but I feel that's an overkill. Instead I just use the average size.
		childIdx = 0;
		UINT32 layoutSize = (UINT32)Math::CeilToInt(totalOptimalSize / (float)numLayouts);
		for(auto& child : mChildren)
		{
			if(child.isLayout())
			{
				elementSizes[childIdx] += layoutSize;
				totalOptimalSize += layoutSize;
			}
			childIdx++;
		}

		// Determine weight scale for every element
		childIdx = 0;
		float invOptimalSize = 1.0f / totalOptimalSize;
		for(auto& child : mChildren)
		{
			elementScaleWeights[childIdx] = invOptimalSize * elementSizes[childIdx];

			childIdx++;
		}

		// Our optimal size is larger than maximum allowed, so we need to reduce size of some elements
		if(totalOptimalSize > width)
		{
			UINT32 extraSize = totalOptimalSize - width;
			UINT32 remainingSize = extraSize;

			// Iterate until we reduce everything so it fits, while maintaining
			// equal average sizes 
			while(remainingSize > 0 && numNonClampedElements > 0)
			{
				UINT32 totalRemainingSize = remainingSize;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
						continue;

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

			// Flexible spaces always expand to fill up all unused space, so we have a special case for them
			if(numFlexibleSpaces > 0)
			{
				float avgSize = remainingSize / (float)numFlexibleSpaces;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
						continue;

					UINT32 extraWidth = std::min((UINT32)Math::CeilToInt(avgSize), remainingSize);
					UINT32 elementWidth = elementSizes[childIdx] + extraWidth;

					// Clamp if needed
					if(child.isFlexibleSpace())
					{
						processedElements[childIdx] = true;
						elementSizes[childIdx] = elementWidth;

						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraWidth);
					}

					childIdx++;
				}
			}
			else // Otherwise, just expand individual elements, as well as their constraints allow it (equal to reduce algorithm above)
			{
				// Iterate until we reduce everything so it fits, while maintaining
				// equal average sizes 
				while(remainingSize > 0 && numNonClampedElements > 0)
				{
					UINT32 totalRemainingSize = remainingSize;

					childIdx = 0;
					for(auto& child : mChildren)
					{
						if(processedElements[childIdx])
							continue;

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

				const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();
				if(layoutOptions.fixedHeight)
					child.element->_setHeight(layoutOptions.height);
				else
				{
					UINT32 optimalHeight = child.element->_getOptimalHeight();

					if(layoutOptions.minHeight > 0)
						optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

					if(layoutOptions.maxHeight > 0)
						optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);

					child.element->_setHeight(optimalHeight);
				}

				UINT32 yOffset = (UINT32)Math::CeilToInt((height - child.element->_getHeight()) * 0.5f);

				Int2 offset(x + xOffset, y + yOffset);
				child.element->_setOffset(offset);
				child.element->_setDepth(depth);

				UINT32 clippedWidth = (UINT32)std::min((INT32)child.element->_getWidth(), (INT32)width - offset.x);
				UINT32 clippedHeight = (UINT32)std::min((INT32)child.element->_getHeight(), (INT32)height - offset.y);

				child.element->_setClipRect(Rect(0, 0, clippedWidth, clippedHeight));
			}
			else if(child.isLayout())
			{
				child.layout->_update(x + xOffset, y, elementWidth, height, depth);
			}

			xOffset += elementWidth;
			childIdx++;
		}

		CM_DELETE_ARRAY(processedElements, bool, (UINT32)mChildren.size(), ScratchAlloc);
		CM_DELETE_ARRAY(elementSizes, UINT32, (UINT32)mChildren.size(), ScratchAlloc);
		CM_DELETE_ARRAY(elementScaleWeights, float, (UINT32)mChildren.size(), ScratchAlloc);
	}
}