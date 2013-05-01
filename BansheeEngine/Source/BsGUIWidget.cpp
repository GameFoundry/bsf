#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
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
				:numQuads(0), quadOffset(0), vertices(nullptr),
				uvs(nullptr), indices(nullptr)
			{ }

			UINT32 numQuads;
			UINT32 quadOffset;
			Vector2* vertices;
			Vector2* uvs;
			UINT32* indices;
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
			renderElem.second.meshData = std::shared_ptr<MeshData>(CM_NEW(MeshData, PoolAlloc) MeshData(),
				&MemAllocDeleter<MeshData, PoolAlloc>::deleter);
			renderElem.second.vertices = renderElem.second.meshData->addPositionsVec2(renderElem.second.numQuads * 4);
			renderElem.second.uvs = renderElem.second.meshData->addUV0(renderElem.second.numQuads * 4);
			renderElem.second.indices = renderElem.second.meshData->addIndices32(renderElem.second.numQuads * 6);
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

				Vector2* vertices = meshDataPerRenderElement[meshGroup].vertices;
				Vector2* uvs = meshDataPerRenderElement[meshGroup].uvs;
				UINT32* indices = meshDataPerRenderElement[meshGroup].indices;
				UINT32 startingQuad = meshDataPerRenderElement[meshGroup].quadOffset;
				UINT32 maxNumQuads = meshDataPerRenderElement[meshGroup].numQuads;

				elem->fillBuffer(vertices, uvs, indices, startingQuad, maxNumQuads, i);

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
			mCachedMeshes[meshIdx]->setMeshData(renderElem.second.meshData);
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
			material->setFloat("halfViewportWidth", camera->getViewport()->getWidth() * 0.5f);
			material->setFloat("halfViewportHeight", camera->getViewport()->getHeight() * 0.5f);
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