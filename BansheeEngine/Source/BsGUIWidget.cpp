#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
#include "CmApplication.h"
#include "CmDeferredRenderContext.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmMesh.h"
#include "BsCamera.h"
#include "CmViewport.h"
#include "CmSceneObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(const HSceneObject& parent)
		:Overlay(parent), mSkin(nullptr)
	{
		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		GUIManager::instance().unregisterWidget(this);

		for(auto& elem : mElements)
		{
			GUIElement::destroy(elem);
		}

		mElements.clear();
	}

	void GUIWidget::registerElement(GUIElement* elem)
	{
		mElements.push_back(elem);
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

	void GUIWidget::mouseEvent(const GUIMouseEvent& ev)
	{

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
			return a->getDepth() > b->getDepth();
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

	void GUIWidget::updateBounds() const
	{
		mCachedBounds.clear();

		const Matrix4& worldTfrm = SO()->getWorldTfrm();

		for(auto& elem : mElements)
		{
			ORect elemBounds(elem->getBounds());
			elemBounds.applyTransform(worldTfrm);

			mCachedBounds.push_back(std::make_pair(elemBounds, elem));
		}
	}

	void GUIWidget::render(const Camera* camera, RenderContext& renderContext) const
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
			float invViewportWidth = 1.0f / (camera->getViewport()->getWidth() * 0.5f);
			float invViewportHeight = 1.0f / (camera->getViewport()->getHeight() * 0.5f);

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