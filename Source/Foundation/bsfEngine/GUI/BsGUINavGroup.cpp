//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "GUI/BsGUINavGroup.h"
#include "GUI/BsGUIElement.h"
#include "GUI/BsGUIManager.h"

namespace bs
{
	SPtr<GUINavGroup> GUINavGroup::create()
	{
		return bs_shared_ptr_new<GUINavGroup>();
	}

	void GUINavGroup::focusFirst()
	{
		if(mOrderedElements.empty())
			return;

		// Find first element with an explicit index, if one exists
		auto iterStart = mOrderedElements.begin();
		if(iterStart->first != 0)
		{
			iterStart->second->setFocus(true);
			return;
		}

		// Otherwise look for top-left element without an explicit index
		focusTopLeft();
	}

	void GUINavGroup::focusNext(GUIElement* anchor)
	{
		// Nothing currently in focus
		if(!anchor)
		{
			focusFirst();
			return;
		}

		const INT32 tabIdx = mElements[anchor];

		// Find next element using the explicit index
		if(tabIdx != 0)
		{
			auto iterFind = mOrderedElements.lower_bound(tabIdx);
			while(iterFind->second != anchor)
				++iterFind;

			++iterFind;

			// Reached the end, wrap around
			if(iterFind == mOrderedElements.end())
			{
				focusFirst();
				return;
			}

			// If a next element with an explicit index exists, select it
			if(iterFind->first != 0)
			{
				iterFind->second->setFocus(true);
				return;
			}

			// Select top-left element with no tab index
			focusTopLeft();
			return;
		}

		// Find next element to focus on, to the right of the current element, or left-most element below it
		{
			const Rect2I focusedElemBounds = anchor->_getClippedBounds();
			INT32 nearestX = std::numeric_limits<INT32>::max();
			INT32 nearestX0 = std::numeric_limits<INT32>::max();
			INT32 nearestY = std::numeric_limits<INT32>::max();
			
			GUIElement* nextElement = nullptr;
			const auto unindexedRange = mOrderedElements.equal_range(0);
			for (auto iter = unindexedRange.first; iter != unindexedRange.second; ++iter)
			{
				GUIElement* element = iter->second;

				const bool acceptsKeyFocus = element->getOptionFlags().isSet(GUIElementOption::AcceptsKeyFocus);
				if(!acceptsKeyFocus || !element->_isVisible() || element->_isDisabled())
					continue;

				if(element == anchor)
					continue;

				const Rect2I elemBounds = element->_getClippedBounds();
				const bool isFullyClipped = element->_getClippedBounds().width == 0 ||
					element->_getClippedBounds().height == 0;

				if(isFullyClipped)
					continue;

				const INT32 yDiff = elemBounds.y - focusedElemBounds.y;

				// First look for element at the same height but to the right
				if(yDiff == 0)
				{
					// Note: We're purposely ignoring elements at the same exact position, as the tab focus would then just
					// ping-pong between the two elements, and we'd have to keep a list of previously visited elements in
					// order to avoid the issue.
					if(elemBounds.x > focusedElemBounds.x)
					{
						const INT32 xDiff = elemBounds.x - focusedElemBounds.x;
						if (xDiff < nearestX0)
						{
							nearestX0 = xDiff;
							nextElement = element;
						}

						nearestY = 0;
					}
				}
				// Otherwise find left-most element that's closest below the current element
				else if(yDiff > 0 && yDiff <= nearestY)
				{
					if(yDiff < nearestY)
					{
						nearestX = elemBounds.x;
						nearestY = yDiff;
						nextElement = element;
					}
					else
					{
						if(elemBounds.x < nearestX)
						{
							nearestX = elemBounds.x;
							nextElement = element;
						}
					}
				}
			}

			if(nextElement)
			{
				nextElement->setFocus(true);
				return;
			}

			// No more elements with no tab index. Check elements with positive tab index
			const auto iterAfterUnindexed = unindexedRange.second;
			if(iterAfterUnindexed != mOrderedElements.end())
			{
				iterAfterUnindexed->second->setFocus(true);
				return;
			}

			// Reached the end, wrap around
			focusFirst();
		}
	}

	void GUINavGroup::focusTopLeft()
	{
		UINT32 lowestDist = std::numeric_limits<UINT32>::max();
		GUIElement* topLeftElement = nullptr;

		// Grab only elements without an explicit index
		const auto unindexedRange = mOrderedElements.equal_range(0);
		for(auto iter = unindexedRange.first; iter != unindexedRange.second; ++iter)
		{
			GUIElement* element = iter->second;

			// Ignore elements that are hidden, disabled or just don't accept input focus
			const bool acceptsKeyFocus = element->getOptionFlags().isSet(GUIElementOption::AcceptsKeyFocus);
			if(!acceptsKeyFocus || !element->_isVisible() || element->_isDisabled())
				continue;

			// Ignore elements that have been fully clipped
			const Rect2I elemBounds = element->_getClippedBounds();
			if(elemBounds.width == 0 || elemBounds.height == 0)
				continue;

			Vector2I elementPos(elemBounds.x, elemBounds.y);

			const UINT32 dist = elementPos.squaredLength();
			if (dist < lowestDist)
			{
				lowestDist = dist;
				topLeftElement = element;
			}
		}

		if (topLeftElement)
			topLeftElement->setFocus(true);
	}
	
	void GUINavGroup::registerElement(GUIElement* element, INT32 tabIdx)
	{
		mElements[element] = tabIdx;
		mOrderedElements.insert(std::make_pair(tabIdx, element));
	}

	void GUINavGroup::setIndex(GUIElement* element, INT32 tabIdx)
	{
		const auto iterFind = mElements.find(element);
		assert(iterFind != mElements.end());

		const INT32 existingTabIdx = iterFind->second;
		mElements[element] = tabIdx;

		const auto iterPair = mOrderedElements.equal_range(existingTabIdx);
		for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
		{
			if(iter->second == element)
			{
				mOrderedElements.erase(iter);
				break;
			}
		}

		mOrderedElements.insert(std::make_pair(tabIdx, element));
	}

	void GUINavGroup::unregisterElement(GUIElement* element)
	{
		const auto iterFind = mElements.find(element);
		if(iterFind == mElements.end())
			return;

		const INT32 existingTabIdx = iterFind->second;
		const auto iterPair = mOrderedElements.equal_range(existingTabIdx);
		for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
		{
			if(iter->second == element)
			{
				mOrderedElements.erase(iter);
				break;
			}
		}

		mElements.erase(element);
	}
}
