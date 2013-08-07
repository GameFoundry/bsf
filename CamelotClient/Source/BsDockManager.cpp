#include "BsDockManager.h"
#include "BsEditorWidgetContainer.h"
#include "CmMath.h"
#include "CmException.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 DockManager::DockContainer::SliderSize = 4;

	DockManager::DockContainer::DockContainer()
		:mIsLeaf(false), mWidgets(nullptr), mX(0), mY(0), mWidth(0), mHeight(0), mSplitPosition(0.5f),
		mIsHorizontal(false)
	{
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;
	}

	DockManager::DockContainer::~DockContainer()
	{
		if(mIsLeaf && mWidgets != nullptr)
			cm_delete(mWidgets);

		if(!mIsLeaf)
		{
			if(mChildren[0] != nullptr)
				cm_delete(mChildren[0]);

			if(mChildren[1] != nullptr)
				cm_delete(mChildren[1]);
		}

		// TODO - Clean up slider
	}

	void DockManager::DockContainer::setArea(CM::INT32 x, CM::INT32 y, UINT32 width, UINT32 height)
	{
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;

		if(mIsLeaf)
		{
			if(mWidgets != nullptr)
			{
				mWidgets->setPosition(x, y);
				mWidgets->setSize(width, height);
			}
		}
		else if(mChildren[0] != nullptr && mChildren[1] != nullptr)
		{
			if(mIsHorizontal)
			{
				UINT32 remainingSize = (UINT32)std::max(0, (INT32)mHeight - (INT32)SliderSize);
				UINT32 sizeTop = Math::FloorToInt(remainingSize * mSplitPosition);
				UINT32 sizeBottom = remainingSize - sizeTop;

				mChildren[0]->setArea(mX, mY, mWidth, sizeTop);
				mChildren[1]->setArea(mX, mY + sizeTop + SliderSize, mWidth, sizeBottom);

				// TODO - Set slider position
			}
			else
			{
				UINT32 remainingSize = (UINT32)std::max(0, (INT32)mWidth - (INT32)SliderSize);
				UINT32 sizeLeft = Math::FloorToInt(remainingSize * mSplitPosition);
				UINT32 sizeRight = remainingSize - sizeLeft;

				mChildren[0]->setArea(mX, mY, sizeLeft, mHeight);
				mChildren[1]->setArea(mX + sizeLeft + SliderSize, mY, sizeRight, mHeight);

				// TODO - Set slider position
			}
		}
	}

	void DockManager::DockContainer::makeLeaf(GUIWidget* widgetParent, EditorWidget* widget)
	{
		mIsLeaf = true;
		mWidgets = cm_new<EditorWidgetContainer>(widgetParent);

		mWidgets->add(*widget);
		mWidgets->setPosition(mX, mY);
		mWidgets->setSize(mWidth, mHeight);
	}

	void DockManager::DockContainer::addLeft(BS::GUIWidget* widgetParent, EditorWidget* widget)
	{
		splitContainer(widgetParent, widget, false, true);
	}

	void DockManager::DockContainer::addRight(BS::GUIWidget* widgetParent, EditorWidget* widget)
	{
		splitContainer(widgetParent, widget, false, false);
	}

	void DockManager::DockContainer::addTop(BS::GUIWidget* widgetParent, EditorWidget* widget)
	{
		splitContainer(widgetParent, widget, true, true);
	}

	void DockManager::DockContainer::addBottom(BS::GUIWidget* widgetParent, EditorWidget* widget)
	{
		splitContainer(widgetParent, widget, true, false);
	}

	void DockManager::DockContainer::splitContainer(BS::GUIWidget* widgetParent, EditorWidget* widget, bool horizontal, bool newChildIsFirst)
	{
		UINT32 idxA = newChildIsFirst ? 0 : 1;
		UINT32 idxB = (idxA + 1) % 2;

		mChildren[idxA] = cm_new<DockContainer>();
		mChildren[idxB] = cm_new<DockContainer>(*this);

		mChildren[idxA]->makeLeaf(widgetParent, widget);

		mIsLeaf = false;
		mIsHorizontal = horizontal;
		mWidgets = nullptr;
		mSplitPosition = 0.5f;

		// TODO - Add slider

		setArea(mX, mY, mWidth, mHeight);
	}

	DockManager::DockContainer* DockManager::DockContainer::find(EditorWidgetContainer* widgetContainer)
	{
		if(mIsLeaf)
		{
			if(mWidgets == widgetContainer)
				return this;
			else
				return nullptr;
		}
		else
		{
			if(mChildren[0] != nullptr && mChildren[0]->find(widgetContainer) != nullptr)
				return mChildren[0];

			if(mChildren[1] != nullptr && mChildren[1]->find(widgetContainer) != nullptr)
				return mChildren[1];
		}

		return nullptr;
	}

	DockManager::DockManager(BS::GUIWidget* parent)
		:mParent(parent)
	{

	}

	DockManager::~DockManager()
	{

	}

	void DockManager::insert(EditorWidgetContainer* relativeTo, EditorWidget* widgetToInsert, DockLocation location)
	{
		if(relativeTo != nullptr)
		{
			DockContainer* container = mRootContainer.find(relativeTo);
			if(container == nullptr)
				CM_EXCEPT(InternalErrorException, "Cannot find the wanted widget container relative to which the widget should be inserted.");

			switch(location)
			{
			case DockLocation::Left:
				container->addLeft(mParent, widgetToInsert);
				break;
			case DockLocation::Right:
				container->addRight(mParent, widgetToInsert);
				break;
			case DockLocation::Top:
				container->addTop(mParent, widgetToInsert);
				break;
			case DockLocation::Bottom:
				container->addBottom(mParent, widgetToInsert);
				break;
			}
		}
		else
		{
			if(mRootContainer.mWidgets != nullptr)
				CM_EXCEPT(InternalErrorException, "Trying to insert a widget into dock manager root container but one already exists.");

			mRootContainer.makeLeaf(mParent, widgetToInsert);
		}
	}

	void DockManager::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mRootContainer.setArea(x, y, width, height);
	}
}