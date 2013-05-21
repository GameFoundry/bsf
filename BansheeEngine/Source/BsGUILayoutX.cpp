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

		bool* processedElements = CM_NEW_ARRAY(bool, (UINT32)mChildren.size(), ScratchAlloc);
		memset(processedElements, 0, mChildren.size() * sizeof(bool));

		UINT32* elementSizes = CM_NEW_ARRAY(UINT32, (UINT32)mChildren.size(), ScratchAlloc);
		memset(elementSizes, 0, mChildren.size() * sizeof(UINT32));

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
				// Layout use any size that's available (quite possibly none), but it might be better to give them a certain minimum size.
				numNonClampedElements++;
			}
			else if(child.isFlexibleSpace())
			{
				numFlexibleSpaces++;
				numNonClampedElements++;
			}

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
				float avgSize = remainingSize / (float)numNonClampedElements;

				childIdx = 0;
				for(auto& child : mChildren)
				{
					if(processedElements[childIdx])
						continue;

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
					float avgSize = remainingSize / (float)numNonClampedElements;

					childIdx = 0;
					for(auto& child : mChildren)
					{
						if(processedElements[childIdx])
							continue;

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

		//// Calculate flexible space sizes
		//std::vector<UINT32> flexibleSpaceSizes;
		//UINT32 minimalTotalSize = 0;
		//for(auto& child : mChildren)
		//{
		//	if(child.isFlexibleSpace())
		//	{
		//		flexibleSpaceSizes.push_back(0);
		//	}
		//	else if(child.isFixedSpace())
		//	{
		//		minimalTotalSize += child.space->getSize();
		//	}
		//	else if(child.isElement())
		//	{
		//		const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

		//		if(layoutOptions.fixedWidth)
		//			minimalTotalSize += layoutOptions.width;
		//		else
		//		{
		//			UINT32 optimalWidth = child.element->_getOptimalWidth();

		//			if(layoutOptions.minHeight > 0)
		//				optimalWidth = std::max(layoutOptions.minWidth, optimalWidth);

		//			if(layoutOptions.maxHeight > 0)
		//				optimalWidth = std::min(layoutOptions.maxWidth, optimalWidth);

		//			minimalTotalSize += optimalWidth;
		//		}
		//	}
		//}

		//UINT32 remainingFlexSpaceSize = (UINT32)std::max(0, (INT32)width - (INT32)minimalTotalSize);
		//float avgFlexibleSpaceSize = remainingFlexSpaceSize / (float)flexibleSpaceSizes.size();
		//
		//for(size_t i = 0; i < flexibleSpaceSizes.size(); i++)
		//{
		//	UINT32 spaceSize = (UINT32)Math::CeilToInt(avgFlexibleSpaceSize);
		//	spaceSize = std::min(remainingFlexSpaceSize, spaceSize);

		//	remainingFlexSpaceSize -= spaceSize;
		//	flexibleSpaceSizes[i] = spaceSize;
		//}

		//// Get a basic estimate of the average width
		//UINT32 totalWidth = 0;
		//UINT32 numFreeElems = 0;
		//float avgWidth = 0.0f;
		//UINT32 flexibleSpaceIdx = 0;
		//for(auto& child : mChildren)
		//{
		//	if(child.isElement())
		//	{
		//		const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

		//		if(layoutOptions.fixedWidth)
		//		{
		//			totalWidth += layoutOptions.width;
		//		}
		//		else
		//		{
		//			numFreeElems++;
		//		}
		//	}
		//	else if(child.isFixedSpace())
		//	{
		//		totalWidth += child.space->getSize();
		//	}
		//	else if(child.isFlexibleSpace())
		//	{
		//		totalWidth += flexibleSpaceSizes[flexibleSpaceIdx];
		//		flexibleSpaceIdx++;
		//	}
		//	else
		//	{
		//		numFreeElems++;
		//	}
		//}

		//UINT32 leftoverWidth = (UINT32)std::max(0, (INT32)width - (INT32)totalWidth);
		//float averageWidth = leftoverWidth / (float) numFreeElems;

		//// Only assign elements with fixed, or clamped width
		//bool* processedElements = CM_NEW_ARRAY(bool, (UINT32)mChildren.size(), ScratchAlloc);
		//memset(processedElements, 0, mChildren.size() * sizeof(bool));

		//UINT32 childIdx = 0;
		//for(auto& child : mChildren)
		//{
		//	if(child.isElement())
		//	{
		//		const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

		//		UINT32 elementWidth = 0;

		//		if(layoutOptions.fixedWidth)
		//		{
		//			elementWidth = layoutOptions.width;
		//			processedElements[childIdx] = true;
		//		}
		//		else
		//		{
		//			UINT32 availableWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

		//			// Clamp average to min max
		//			if(layoutOptions.minWidth > 0 && availableWidth < layoutOptions.minWidth)
		//			{
		//				elementWidth = layoutOptions.minWidth;
		//				leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
		//				
		//				processedElements[childIdx] = true;
		//				numFreeElems--;
		//			}
		//			else if(layoutOptions.maxWidth > 0 && availableWidth > layoutOptions.maxWidth)
		//			{
		//				elementWidth = layoutOptions.maxWidth;
		//				leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);
		//				
		//				processedElements[childIdx] = true;
		//				numFreeElems--;
		//			}
		//		}

		//		child.element->_setWidth(elementWidth);
		//	}

		//	childIdx++;
		//}

		//averageWidth = leftoverWidth / (float) numFreeElems;

		//// Assign free scaling elements now that we have a good estimate on average width
		//// Note: Our average value is JUST an estimate. It's hard to predict the actual value because
		//// of min/max constraints and would probably require a few iterations to get a somewhat accurate value,
		//// but I don't think that is worth the trouble. For that reason min/max clamping that might happen in 
		//// the next block might cause elements to either be a bit too small or too large for the layout.
		//// It is the assumption that such clamping won't happen often.
		//// 
		//// Also calculate offset and element height.
		//UINT32 xOffset = 0;
		//flexibleSpaceIdx = 0;
		//childIdx = 0;
		//for(auto& child : mChildren)
		//{
		//	if(child.isElement())
		//	{
		//		const GUILayoutOptions& layoutOptions = child.element->_getLayoutOptions();

		//		if(!processedElements[childIdx])
		//		{
		//			UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);

		//			if(layoutOptions.minWidth > 0 && elementWidth < layoutOptions.minWidth)
		//				elementWidth = layoutOptions.minWidth;
		//			else if(layoutOptions.maxWidth > 0 && elementWidth > layoutOptions.maxWidth)
		//				elementWidth = layoutOptions.maxWidth;

		//			leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);

		//			child.element->_setWidth(elementWidth);
		//		}

		//		if(layoutOptions.fixedHeight)
		//			child.element->_setHeight(layoutOptions.height);
		//		else
		//		{
		//			UINT32 optimalHeight = child.element->_getOptimalHeight();

		//			if(layoutOptions.minHeight > 0)
		//				optimalHeight = std::max(layoutOptions.minHeight, optimalHeight);

		//			if(layoutOptions.maxHeight > 0)
		//				optimalHeight = std::min(layoutOptions.maxHeight, optimalHeight);

		//			child.element->_setHeight(optimalHeight);
		//		}

		//		UINT32 yOffset = (UINT32)Math::CeilToInt((height - child.element->_getHeight()) * 0.5f);

		//		Int2 offset(x + xOffset, y + yOffset);
		//		child.element->_setOffset(offset);
		//		child.element->_setDepth(depth);

		//		UINT32 clippedWidth = (UINT32)std::min((INT32)child.element->_getWidth(), (INT32)width - offset.x);
		//		UINT32 clippedHeight = (UINT32)std::min((INT32)child.element->_getHeight(), (INT32)height - offset.y);

		//		child.element->_setClipRect(Rect(0, 0, clippedWidth, clippedHeight));

		//		xOffset += child.element->_getWidth();
		//	}
		//	else if(child.isLayout())
		//	{
		//		UINT32 elementWidth = std::min((UINT32)Math::CeilToInt(averageWidth), leftoverWidth);
		//		leftoverWidth = (UINT32)std::max(0, (INT32)leftoverWidth - (INT32)elementWidth);

		//		child.layout->_update(x + xOffset, y, elementWidth, height, depth);

		//		xOffset += elementWidth;
		//	}
		//	else if(child.isFixedSpace())
		//	{
		//		xOffset += child.space->getSize();
		//	}
		//	else if(child.isFlexibleSpace())
		//	{
		//		xOffset += flexibleSpaceSizes[flexibleSpaceIdx];
		//		flexibleSpaceIdx++;
		//	}

		//	childIdx++;
		//}

		//CM_DELETE_ARRAY(processedElements, bool, (UINT32)mChildren.size(), ScratchAlloc);
	}
}