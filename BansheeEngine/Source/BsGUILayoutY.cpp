#include "BsGUILayoutY.h"
#include "BsGUIElement.h"
#include "BsGUISpace.h"
#include "BsMath.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	GUILayoutY::GUILayoutY(GUIArea* parentArea)
		:GUILayout(parentArea)
	{ }

	Vector2I GUILayoutY::_calculateOptimalLayoutSize() const
	{
		if (mIsDisabled)
			return Vector2I(0, 0);

		UINT32 optimalWidth = 0;
		UINT32 optimalHeight = 0;

		for (auto& child : mChildren)
		{
			Vector2I optimalSize = child->_calculateOptimalLayoutSize();
			
			if (child->_getType() == GUIElementBase::Type::FixedSpace)
				optimalSize.x = 0;

			UINT32 paddingX = child->_getPadding().left + child->_getPadding().right;
			UINT32 paddingY = child->_getPadding().top + child->_getPadding().bottom;

			optimalHeight += optimalSize.y + paddingY;
			optimalWidth = std::max((UINT32)optimalSize.x, optimalWidth + paddingX);
		}

		return Vector2I(optimalWidth, optimalHeight);
	}

	void GUILayoutY::_updateOptimalLayoutSizes()
	{
		// Update all children first, otherwise we can't determine our own optimal size
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
				GUIFixedSpace* fixedSpace = static_cast<GUIFixedSpace*>(child);
				optimalHeight = fixedSpace->_calculateOptimalLayoutSize().y;
			}
			else if(child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

				Vector2I optimalSize = child->_calculateOptimalLayoutSize();
				optimalWidth = optimalSize.x;
				optimalHeight = optimalSize.y;
			}
			else if(child->_getType() == GUIElementBase::Type::Layout)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);
				optimalWidth = layout->_getOptimalSize().x;
				optimalHeight = layout->_getOptimalSize().y;
			}

			UINT32 paddingX = child->_getPadding().left + child->_getPadding().right;
			UINT32 paddingY = child->_getPadding().top + child->_getPadding().bottom;

			mOptimalSizes[childIdx].y = optimalHeight;
			mOptimalHeight += optimalHeight + paddingY;

			mOptimalSizes[childIdx].x = optimalWidth;
			mOptimalWidth = std::max(mOptimalWidth, optimalWidth + paddingX);

			childIdx++;
		}
	}

	void GUILayoutY::_getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, const Vector<Vector2I>& optimalSizes) const
	{
		assert(mChildren.size() == numElements);

		UINT32 totalOptimalSize = _getOptimalSize().y;
		UINT32 totalNonClampedSize = 0;
		UINT32 numNonClampedElements = 0;
		UINT32 numFlexibleSpaces = 0;

		bool* processedElements = nullptr;
		float* elementScaleWeights = nullptr;

		if (mChildren.size() > 0)
		{
			processedElements = stackAllocN<bool>((UINT32)mChildren.size());
			memset(processedElements, 0, mChildren.size() * sizeof(bool));

			elementScaleWeights = stackAllocN<float>((UINT32)mChildren.size());
			memset(elementScaleWeights, 0, mChildren.size() * sizeof(float));
		}

		// Set initial sizes, count number of children per type and mark fixed elements as already processed
		UINT32 childIdx = 0;
		for (auto& child : mChildren)
		{
			elementAreas[childIdx].height = optimalSizes[childIdx].y;

			if (child->_getType() == GUIElementBase::Type::FixedSpace)
			{
				processedElements[childIdx] = true;
			}
			else if (child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

				if (layoutOptions.fixedHeight)
					processedElements[childIdx] = true;
				else
				{
					numNonClampedElements++;
					totalNonClampedSize += elementAreas[childIdx].height;
				}
			}
			else if (child->_getType() == GUIElementBase::Type::Layout)
			{
				numNonClampedElements++;
				totalNonClampedSize += elementAreas[childIdx].height;
			}
			else if (child->_getType() == GUIElementBase::Type::FlexibleSpace)
			{
				numFlexibleSpaces++;
				numNonClampedElements++;
			}

			childIdx++;
		}

		// If there is some room left, calculate flexible space sizes (since they will fill up all that extra room)
		if (height > totalOptimalSize)
		{
			UINT32 extraSize = height - totalOptimalSize;
			UINT32 remainingSize = extraSize;

			// Flexible spaces always expand to fill up all unused space
			if (numFlexibleSpaces > 0)
			{
				float avgSize = remainingSize / (float)numFlexibleSpaces;

				childIdx = 0;
				for (auto& child : mChildren)
				{
					if (processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					UINT32 extraHeight = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementHeight = elementAreas[childIdx].height + extraHeight;

					// Clamp if needed
					if (child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						processedElements[childIdx] = true;
						numNonClampedElements--;
						elementAreas[childIdx].height = elementHeight;

						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraHeight);
					}

					childIdx++;
				}

				totalOptimalSize = height;
			}
		}

		// Determine weight scale for every element. When scaling elements up/down they will be scaled based on this weight.
		// Weight is to ensure all elements are scaled fairly, so elements that are large will get effected more than smaller elements.
		childIdx = 0;
		float invOptimalSize = 1.0f / totalNonClampedSize;
		for (auto& child : mChildren)
		{
			if (processedElements[childIdx])
			{
				childIdx++;
				continue;
			}

			elementScaleWeights[childIdx] = invOptimalSize * elementAreas[childIdx].height;

			childIdx++;
		}

		// Our optimal size is larger than maximum allowed, so we need to reduce size of some elements
		if (totalOptimalSize > height)
		{
			UINT32 extraSize = totalOptimalSize - height;
			UINT32 remainingSize = extraSize;

			// Iterate until we reduce everything so it fits, while maintaining
			// equal average sizes using the weights we calculated earlier
			while (remainingSize > 0 && numNonClampedElements > 0)
			{
				UINT32 totalRemainingSize = remainingSize;

				childIdx = 0;
				for (auto& child : mChildren)
				{
					if (processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					float avgSize = totalRemainingSize * elementScaleWeights[childIdx];

					UINT32 extraHeight = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementHeight = (UINT32)std::max(0, (INT32)elementAreas[childIdx].height - (INT32)extraHeight);

					// Clamp if needed
					if (child->_getType() == GUIElementBase::Type::Element)
					{
						GUIElement* element = static_cast<GUIElement*>(child);
						const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

						if (elementHeight == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}
						else if (layoutOptions.minHeight > 0 && elementHeight < layoutOptions.minHeight)
						{
							elementHeight = layoutOptions.minHeight;

							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraHeight = elementAreas[childIdx].height - elementHeight;
						elementAreas[childIdx].height = elementHeight;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraHeight);
					}
					else if (child->_getType() == GUIElementBase::Type::Layout)
					{
						if (elementHeight == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraHeight = elementAreas[childIdx].height - elementHeight;
						elementAreas[childIdx].height = elementHeight;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraHeight);
					}
					else if (child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						elementAreas[childIdx].height = 0;
						processedElements[childIdx] = true;
						numNonClampedElements--;
					}

					childIdx++;
				}
			}
		}
		else // We are smaller than the allowed maximum, so try to expand some elements
		{
			UINT32 extraSize = height - totalOptimalSize;
			UINT32 remainingSize = extraSize;

			// Iterate until we reduce everything so it fits, while maintaining
			// equal average sizes using the weights we calculated earlier
			while (remainingSize > 0 && numNonClampedElements > 0)
			{
				UINT32 totalRemainingSize = remainingSize;

				childIdx = 0;
				for (auto& child : mChildren)
				{
					if (processedElements[childIdx])
					{
						childIdx++;
						continue;
					}

					float avgSize = totalRemainingSize * elementScaleWeights[childIdx];
					UINT32 extraHeight = std::min((UINT32)Math::ceilToInt(avgSize), remainingSize);
					UINT32 elementHeight = elementAreas[childIdx].height + extraHeight;

					// Clamp if needed
					if (child->_getType() == GUIElementBase::Type::Element)
					{
						GUIElement* element = static_cast<GUIElement*>(child);
						const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();

						if (elementHeight == 0)
						{
							processedElements[childIdx] = true;
							numNonClampedElements--;
						}
						else if (layoutOptions.maxHeight > 0 && elementHeight > layoutOptions.maxHeight)
						{
							elementHeight = layoutOptions.maxHeight;

							processedElements[childIdx] = true;
							numNonClampedElements--;
						}

						extraHeight = elementHeight - elementAreas[childIdx].height;
						elementAreas[childIdx].height = elementHeight;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraHeight);
					}
					else if (child->_getType() == GUIElementBase::Type::Layout)
					{
						elementAreas[childIdx].height = elementHeight;
						remainingSize = (UINT32)std::max(0, (INT32)remainingSize - (INT32)extraHeight);
					}
					else if (child->_getType() == GUIElementBase::Type::FlexibleSpace)
					{
						processedElements[childIdx] = true;
						numNonClampedElements--;
					}

					childIdx++;
				}
			}
		}

		if (elementScaleWeights != nullptr)
			stackDeallocLast(elementScaleWeights);

		if (processedElements != nullptr)
			stackDeallocLast(processedElements);

		// Compute offsets and width
		UINT32 yOffset = 0;
		childIdx = 0;

		for (auto& child : mChildren)
		{
			UINT32 elemHeight = elementAreas[childIdx].height;
			yOffset += child->_getPadding().top;

			if (child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);
				UINT32 elemWidth = optimalSizes[childIdx].x;
				const GUILayoutOptions& layoutOptions = element->_getLayoutOptions();
				if (!layoutOptions.fixedWidth)
				{
					elemWidth = width;
					if (layoutOptions.minWidth > 0 && elemWidth < layoutOptions.minWidth)
						elemWidth = layoutOptions.minWidth;

					if (layoutOptions.maxWidth > 0 && elemWidth > layoutOptions.maxWidth)
						elemWidth = layoutOptions.maxWidth;
				}

				elementAreas[childIdx].width = elemWidth;

				UINT32 xPadding = element->_getPadding().left + element->_getPadding().right;
				INT32 xOffset = Math::ceilToInt((INT32)(width - (INT32)(elemWidth + xPadding)) * 0.5f);
				xOffset = std::max(0, xOffset);

				elementAreas[childIdx].x = x + xOffset;
				elementAreas[childIdx].y = y + yOffset;
			}
			else if (child->_getType() == GUIElementBase::Type::Layout)
			{
				elementAreas[childIdx].width = width;
				elementAreas[childIdx].x = x;
				elementAreas[childIdx].y = y + yOffset;
			}

			yOffset += elemHeight + child->_getPadding().bottom;
			childIdx++;
		}
	}

	void GUILayoutY::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 numElements = (UINT32)mChildren.size();
		Rect2I* elementAreas = nullptr;
		
		if (numElements > 0)
			elementAreas = stackConstructN<Rect2I>(numElements);

		_getElementAreas(x, y, width, height, elementAreas, numElements, mOptimalSizes);

		// Now that we have all the areas, actually assign them
		UINT32 childIdx = 0;
		Rect2I* actualSizes = elementAreas; // We re-use the same array

		for(auto& child : mChildren)
		{
			Rect2I childArea = elementAreas[childIdx];

			if(child->_getType() == GUIElementBase::Type::Element)
			{
				GUIElement* element = static_cast<GUIElement*>(child);

				element->setWidth(childArea.width);
				element->setHeight(childArea.height);

				Vector2I offset(childArea.x, childArea.y);
				element->setOffset(offset);
				element->_setWidgetDepth(widgetDepth);
				element->_setAreaDepth(areaDepth);

				Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
				element->_setClipRect(elemClipRect);

				Rect2I newClipRect(offset.x, offset.y, childArea.width, childArea.height);
				newClipRect.clip(clipRect);
				element->_updateLayoutInternal(offset.x, offset.y, childArea.width, childArea.height, newClipRect, widgetDepth, areaDepth);

				actualSizes[childIdx].width = childArea.width + element->_getPadding().left + element->_getPadding().right;
			}
			else if(child->_getType() == GUIElementBase::Type::Layout)
			{
				GUILayout* layout = static_cast<GUILayout*>(child);

				Rect2I newClipRect(childArea.x, childArea.y, width, childArea.height);
				newClipRect.clip(clipRect);
				layout->_updateLayoutInternal(childArea.x, childArea.y, width, childArea.height, newClipRect, widgetDepth, areaDepth);

				actualSizes[childIdx].width = layout->_getActualWidth();
			}
			else
				actualSizes[childIdx].width = childArea.width;

			actualSizes[childIdx].height = childArea.height + child->_getPadding().top + child->_getPadding().bottom;
			childIdx++;
		}

		Vector2I actualSize = _calcActualSize(actualSizes, numElements);
		mActualWidth = (UINT32)actualSize.x;
		mActualHeight = (UINT32)actualSize.y;

		if (elementAreas != nullptr)
			stackDeallocLast(elementAreas);

		_markAsClean();
	}

	Vector2I GUILayoutY::_calcActualSize(Rect2I* elementAreas, UINT32 numElements) const
	{
		Vector2I actualArea;
		for (UINT32 i = 0; i < numElements; i++)
		{
			Rect2I childArea = elementAreas[i];

			actualArea.x = std::max(actualArea.x, childArea.width);
			actualArea.y += childArea.height;
		}

		return actualArea;
	}
}