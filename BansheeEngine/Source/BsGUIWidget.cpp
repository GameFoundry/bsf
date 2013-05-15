#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIArea.h"
#include "CmApplication.h"
#include "CmDeferredRenderContext.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmMesh.h"
#include "CmInt2.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "CmViewport.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(const HSceneObject& parent)
		:Overlay(parent), mSkin(nullptr), mOwnerWindow(nullptr), mWidgetIsDirty(false)
	{
		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		GUIManager::instance().unregisterWidget(this);

		for(auto& elem : mElements)
		{
			GUIElement::destroyInternal(elem);
		}

		for(auto& area : mAreas)
		{
			GUIArea::destroyInternal(area);
		}

		mElements.clear();
	}

	void GUIWidget::initialize(Viewport* target, const RenderWindow* ownerWindow)
	{
		assert(target != nullptr);
		assert(ownerWindow != nullptr);

		if(mOwnerWindow != nullptr)
			CM_EXCEPT(InvalidStateException, "Widget has already been initialized.");

		Overlay::initialize(target);

		mOwnerWindow = ownerWindow;
		mOwnerWindow->onWindowMovedOrResized.connect(boost::bind(&GUIWidget::ownerWindowResized, this, _1));
	}

	void GUIWidget::registerElement(GUIElement* elem)
	{
		assert(elem != nullptr);

		mElements.push_back(elem);

		mWidgetIsDirty = true;
	}

	void GUIWidget::unregisterElement(GUIElement* elem)
	{
		assert(elem != nullptr);

		auto iterFind = std::find(begin(mElements), end(mElements), elem);

		if(iterFind == mElements.end())
			CM_EXCEPT(InvalidParametersException, "Cannot unregister an element that is not registered on this widget.");

		mElements.erase(iterFind);
		mWidgetIsDirty = true;
	}

	void GUIWidget::registerArea(GUIArea* area)
	{
		assert(area != nullptr);

		mAreas.push_back(area);

		mWidgetIsDirty = true;
	}

	void GUIWidget::unregisterArea(GUIArea* area)
	{
		assert(area != nullptr);

		auto iterFind = std::find(begin(mAreas), end(mAreas), area);

		if(iterFind == mAreas.end())
			CM_EXCEPT(InvalidParametersException, "Cannot unregister an area that is not registered on this widget.");

		mAreas.erase(iterFind);
		mWidgetIsDirty = true;
	}

	void GUIWidget::setSkin(const GUISkin* skin)
	{
		mSkin = skin;
	}

	const GUISkin* GUIWidget::getGUISkin() const
	{
		if(mSkin != nullptr)
			return mSkin;
		else
			return &DefaultSkin;
	}

	void GUIWidget::updateMeshes() const
	{
		struct TempMeshData
		{
			TempMeshData()
				:numQuads(0), quadOffset(0)
			{ }

			UINT32 numQuads;
			UINT32 quadOffset;
			HMaterial material;
			std::shared_ptr<MeshData> meshData;
		};

		bool isDirty = mWidgetIsDirty;
		mWidgetIsDirty = false;

		if(!isDirty)
		{
			for(auto& elem : mElements)
			{
				if(elem->isDirty())
				{
					isDirty = true;
					break;
				}
			}
		}

		if(!isDirty) // Nothing to update
			return;

		std::unordered_map<UINT64, TempMeshData> meshDataPerRenderElement;

		// Group meshes based on used materials
		// Determine mesh sizes per group
		for(auto& elem : mElements)
		{
			UINT32 numRenderElems = elem->getNumRenderElements();

			for(UINT32 i = 0; i < numRenderElems; i++)
			{
				const HMaterial& mat = elem->getMaterial(i);

				UINT64 meshGroup = mat->getInternalID(); // TODO - I group based on material ID. So if two widgets used exact copies of the same material
				// this system won't detect it. Find a better way of determining material similarity?

				UINT32 numQuads = elem->getNumQuads(i);
				meshDataPerRenderElement[meshGroup].numQuads += numQuads;
				meshDataPerRenderElement[meshGroup].material = mat;
			}
		}

		// Allocate buffers for each group
		UINT32 numMeshes = 0;
		for(auto& renderElem : meshDataPerRenderElement)
		{
			MeshDataPtr meshData = std::shared_ptr<MeshData>(CM_NEW(MeshData, PoolAlloc) MeshData(renderElem.second.numQuads * 4),
				&MemAllocDeleter<MeshData, PoolAlloc>::deleter);

			meshData->beginDesc();
			meshData->addVertElem(VET_FLOAT2, VES_POSITION);
			meshData->addVertElem(VET_FLOAT2, VES_TEXCOORD);
			meshData->addSubMesh(renderElem.second.numQuads * 6);
			meshData->endDesc();

			renderElem.second.meshData = meshData;
			numMeshes++;
		}

		// TODO - Sorting from scratch every time is not optimal.
		//  If more performance is needed, try re-sorting only modified elements
		//  Sort so that farthest away elements get drawn first (needed due to transparency)
		std::vector<GUIElement*> sortedElements = mElements;
		std::sort(sortedElements.begin(), sortedElements.end(), 
			[](GUIElement* a, GUIElement* b)
		{
			return a->getDepth() < b->getDepth();
		});

		// Fill buffers for each group
		for(auto& elem : sortedElements)
		{
			UINT32 numRenderElems = elem->getNumRenderElements();

			for(UINT32 i = 0; i < numRenderElems; i++)
			{
				const HMaterial& mat = elem->getMaterial(i);
				UINT64 meshGroup = mat->getInternalID(); 
				MeshDataPtr meshData = meshDataPerRenderElement[meshGroup].meshData;

				UINT8* vertices = meshData->getElementData(VES_POSITION);
				UINT8* uvs = meshData->getElementData(VES_TEXCOORD);
				UINT32* indices = meshData->getIndices32();
				UINT32 startingQuad = meshDataPerRenderElement[meshGroup].quadOffset;
				UINT32 maxNumQuads = meshDataPerRenderElement[meshGroup].numQuads;
				UINT32 vertexStride = meshData->getVertexStride();
				UINT32 indexStride = meshData->getIndexElementSize();
				
				elem->fillBuffer(vertices, uvs, indices, startingQuad, maxNumQuads, vertexStride, indexStride, i);
				elem->markAsClean();

				UINT32 numQuads = elem->getNumQuads(i);
				meshDataPerRenderElement[meshGroup].quadOffset += numQuads;
			}
		}

		// Update meshes
		for(UINT32 i = (UINT32)mCachedMeshes.size(); i < numMeshes; i++)
		{
			HMesh newMesh = Mesh::create();
			mCachedMeshes.push_back(newMesh);
			newMesh.waitUntilLoaded();
		}

		while((UINT32)mCachedMeshes.size() > numMeshes && (UINT32)mCachedMeshes.size() > 0)
		{
			mCachedMeshes.erase(mCachedMeshes.end() - 1); // TODO: Destroying meshes as soon as they're not used might be a perf. penalty?
			//  Maybe instead pool the meshes and only actually remove them when a certain number of unused ones exists.
		}

		mCachedMaterials.resize(numMeshes);

		UINT32 meshIdx = 0;
		for(auto& renderElem : meshDataPerRenderElement)
		{
			gMainSyncedRC().writeSubresource(mCachedMeshes[meshIdx].getInternalPtr(), 0, *renderElem.second.meshData);
			gMainSyncedRC().submitToGpu(true); // TODO - Possibly we can avoid this. I don't see a reason we need to wait for the update to complete.

			mCachedMaterials[meshIdx] = renderElem.second.material;

			meshIdx++;
		}

		updateBounds();
	}

	bool GUIWidget::inBounds(const Int2& position) const
	{
		// Technically GUI widget bounds can be larger than the viewport, so make sure we clip to viewport first
		if(!getTarget()->getDimensions().contains(position))
			return false;

		const Matrix4& worldTfrm = SO()->getWorldTfrm();
		Vector3 vecPos((float)position.x, (float)position.y, 0.0f);
		vecPos = worldTfrm.inverse() * vecPos;

		Int2 localPos(Math::RoundToInt(vecPos.x), Math::RoundToInt(vecPos.y));
		return mBounds.contains(localPos);
	}

	void GUIWidget::updateBounds() const
	{
		const Matrix4& worldTfrm = SO()->getWorldTfrm();

		if(mElements.size() > 0)
			mBounds = mElements[0]->getBounds();

		for(auto& elem : mElements)
		{
			Rect elemBounds = elem->getBounds();
			mBounds.encapsulate(elemBounds);
		}
	}

	void GUIWidget::ownerWindowResized(RenderWindow* window)
	{
		for(auto& area : mAreas)
		{
			area->notifyWindowResized(window->getWidth(), window->getHeight());
		}
	}

	void GUIWidget::render(RenderContext& renderContext) const
	{
		// Mesh is re-created every frame. There might be a better approach that only recreates it upon change,
		// but for now it seems like too much hassle for something like GUI that is pretty dynamic anyway.
		updateMeshes();

		// Render the meshes
		UINT32 meshIdx = 0;
		for(auto& mesh : mCachedMeshes)
		{
			HMaterial material = mCachedMaterials[meshIdx];

			// TODO - Possible optimization. I currently divide by width/height inside the shader, while it
			// might be more optimal to just scale the mesh as the resolution changes?
			float invViewportWidth = 1.0f / (getTarget()->getWidth() * 0.5f);
			float invViewportHeight = 1.0f / (getTarget()->getHeight() * 0.5f);

			material->setFloat("invViewportWidth", invViewportWidth);
			material->setFloat("invViewportHeight", invViewportHeight);
			material->setMat4("worldTransform", SO()->getWorldTfrm());

			if(material == nullptr || !material.isLoaded())
				continue;

			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			for(UINT32 i = 0; i < material->getNumPasses(); i++)
			{
				PassPtr pass = material->getPass(i);
				pass->activate(renderContext);

				PassParametersPtr paramsPtr = material->getPassParameters(i);
				pass->bindParameters(renderContext, paramsPtr);

				renderContext.render(mesh->getRenderOperation());
			}

			meshIdx++;
		}
	}
}