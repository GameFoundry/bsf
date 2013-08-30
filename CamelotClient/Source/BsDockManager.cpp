#include "BsDockManager.h"
#include "BsEditorWidgetContainer.h"
#include "CmMath.h"
#include "CmException.h"
#include "CmMesh.h"
#include "CmMaterial.h"
#include "CmVector2.h"
#include "CmRenderQueue.h"
#include "CmApplication.h"
#include "CmRendererManager.h"
#include "CmRenderer.h"
#include "BsBuiltinMaterialManager.h"
#include "BsGUIWidget.h"
#include "BsCamera.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::UINT32 DockManager::DockContainer::SliderSize = 4;
	const CM::Color DockManager::TINT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.22f);
	const CM::Color DockManager::HIGHLIGHT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.42f);

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
		mDropOverlayMat = BuiltinMaterialManager::instance().createDockDropOverlayMaterial();

		RendererManager::instance().getActive()->addRenderCallback(parent->getTarget(), boost::bind(&DockManager::render, this, _1, _2));
	}

	DockManager::~DockManager()
	{

	}

	void DockManager::render(const Viewport* viewport, CM::RenderQueue& renderQueue)
	{
		float invViewportWidth = 1.0f / (viewport->getWidth() * 0.5f);
		float invViewportHeight = 1.0f / (viewport->getHeight() * 0.5f);

		if(mDropOverlayMesh == nullptr || !mDropOverlayMesh.isLoaded() || !mDropOverlayMesh->isInitialized())
			return;

		if(mDropOverlayMat == nullptr || !mDropOverlayMat.isLoaded() || !mDropOverlayMat->isInitialized())
			return;

		mDropOverlayMat->setFloat("invViewportWidth", invViewportWidth);
		mDropOverlayMat->setFloat("invViewportHeight", invViewportHeight);

		mDropOverlayMat->setColor("tintColor", TINT_COLOR);
		mDropOverlayMat->setColor("highlightColor", HIGHLIGHT_COLOR);
		mDropOverlayMat->setColor("highlightActive", Color(0.0f, 0.0f, 0.0f, 0.0f));

		renderQueue.add(mDropOverlayMat, mDropOverlayMesh->getSubMeshData(), Vector3::ZERO);
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

		createDropOverlayMesh(x, y, width, height);
	}

	void DockManager::createDropOverlayMesh(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		const static int spacing = 10;
		const static float innerScale = 0.75f;

		UINT32 outWidth = std::max(0, (INT32)width - spacing * 2);
		UINT32 outHeight = std::max(0, (INT32)height - spacing * 2);
		UINT32 inWidth = (UINT32)Math::FloorToInt(innerScale * outWidth);
		UINT32 inHeight = (UINT32)Math::FloorToInt(innerScale * outHeight);
		INT32 inXOffset = Math::FloorToInt((outWidth - inWidth) * 0.5f);
		INT32 inYOffset = Math::FloorToInt((outHeight - inHeight) * 0.5f);

		Vector2 outTopLeft((float)x, (float)y);
		Vector2 outTopRight((float)(x + outWidth), (float)y);
		Vector2 outBotLeft((float)x, (float)(y + outHeight));
		Vector2 outBotRight((float)(x + outWidth), (float)(y + outHeight));

		Vector2 inTopLeft((float)(x + inXOffset), (float)(y + inYOffset));
		Vector2 inTopRight((float)(x + inXOffset + inWidth), (float)(y + inYOffset));
		Vector2 inBotLeft((float)(x + inXOffset), (float)(y + inYOffset + inHeight));
		Vector2 inBotRight((float)(x + inXOffset + inWidth), (float)(y + inYOffset + inHeight));

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(16);

		meshData->beginDesc();

		meshData->addSubMesh(24, 0, DOT_TRIANGLE_LIST);
		meshData->addVertElem(VET_FLOAT2, VES_POSITION);
		meshData->addVertElem(VET_COLOR, VES_COLOR);

		meshData->endDesc();

		auto vertIter = meshData->getVec2DataIter(VES_POSITION);
		auto colIter = meshData->getDWORDDataIter(VES_COLOR);

		// Top
		Vector2 topOffset((float)spacing, 0.0f);

		Vector2 dbg = outTopLeft + topOffset;
		vertIter.addValue(outTopLeft + topOffset);
		vertIter.addValue(outTopRight + topOffset);
		vertIter.addValue(inTopRight + topOffset);
		vertIter.addValue(inTopLeft + topOffset);

		Color color(1.0f, 0.0f, 0.0f, 0.0f);
		UINT32 color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Bottom
		Vector2 botOffset((float)spacing, (float)spacing * 2.0f);
		vertIter.addValue(inBotLeft + botOffset);
		vertIter.addValue(inBotRight + botOffset);
		vertIter.addValue(outBotRight + botOffset);
		vertIter.addValue(outBotLeft + botOffset);

		color = Color(0.0f, 1.0f, 0.0f, 0.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Left
		Vector2 leftOffset(0.0f, (float)spacing);
		vertIter.addValue(outTopLeft + leftOffset);
		vertIter.addValue(inTopLeft + leftOffset);
		vertIter.addValue(inBotLeft + leftOffset);
		vertIter.addValue(outBotLeft + leftOffset);

		color = Color(0.0f, 0.0f, 1.0f, 0.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Right
		Vector2 rightOffset((float)spacing * 2.0f, (float)spacing);
		vertIter.addValue(inTopRight + rightOffset);
		vertIter.addValue(outTopRight + rightOffset);
		vertIter.addValue(outBotRight + rightOffset);
		vertIter.addValue(inBotRight + rightOffset);

		color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		UINT32* indexData = meshData->getIndices32();

		// Top
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;

		indexData[3] = 0;
		indexData[4] = 2;
		indexData[5] = 3;

		// Bottom
		indexData[6] = 4;
		indexData[7] = 5;
		indexData[8] = 6;

		indexData[9] = 4;
		indexData[10] = 6;
		indexData[11] = 7;

		// Left
		indexData[12] = 8;
		indexData[13] = 9;
		indexData[14] = 10;

		indexData[15] = 8;
		indexData[16] = 10;
		indexData[17] = 11;

		// Right
		indexData[18] = 12;
		indexData[19] = 13;
		indexData[20] = 14;

		indexData[21] = 12;
		indexData[22] = 14;
		indexData[23] = 15;

		mDropOverlayMesh = Mesh::create();

		gMainSyncedCA().writeSubresource(mDropOverlayMesh.getInternalPtr(), 0, *meshData);
		gMainSyncedCA().submitToCoreThread(true);
	}
}