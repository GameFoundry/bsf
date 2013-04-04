#include "CmGUIWidget.h"
#include "CmGUIManager.h"
#include "CmGUISkin.h"
#include "CmGUILabel.h"
#include "CmDeferredRenderContext.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmMesh.h"

namespace CamelotEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(GameObjectPtr parent)
		:Overlay(parent), mSkin(nullptr)
	{
		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		GUIManager::instance().unregisterWidget(this);

		for(auto& elem : mElements)
		{
			CM_DELETE(elem, GUIElement, GUIAlloc);
		}

		mElements.clear();
	}

	GUILabel* GUIWidget::addLabel(const String& text)
	{
		GUILabel* label = CM_NEW(GUILabel, GUIAlloc) GUILabel(this, text, getGUISkin());
		mElements.push_back(label);

		return label;
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
				:numQuads(0), quadOffset(0), vertices(nullptr),
				uvs(nullptr), indices(nullptr)
			{ }

			UINT32 numQuads;
			UINT32 quadOffset;
			Vector2* vertices;
			Vector2* uvs;
			UINT32* indices;
			MaterialHandle material;
		};

		std::unordered_map<UINT64, TempMeshData> meshDataPerRenderElement;

		// Group meshes based on used materials
		// Determine mesh sizes per group
		for(auto& elem : mElements)
		{
			UINT32 numRenderElems = elem->getNumRenderElements();

			for(UINT32 i = 0; i < numRenderElems; i++)
			{
				const MaterialHandle& mat = elem->getMaterial(i);

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
			renderElem.second.vertices = new Vector2[renderElem.second.numQuads * 4];
			renderElem.second.uvs = new Vector2[renderElem.second.numQuads * 4];
			renderElem.second.indices = new UINT32[renderElem.second.numQuads * 6];
			numMeshes++;
		}

		// Fill buffers for each group
		for(auto& elem : mElements)
		{
			UINT32 numRenderElems = elem->getNumRenderElements();

			for(UINT32 i = 0; i < numRenderElems; i++)
			{
				const MaterialHandle& mat = elem->getMaterial(i);
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
			MeshHandle newMesh = Mesh::create();
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
			std::shared_ptr<MeshData> meshData(new MeshData());

			meshData->setPositions(renderElem.second.vertices, renderElem.second.numQuads * 4);
			meshData->setUV0(renderElem.second.uvs, renderElem.second.numQuads * 4);
			meshData->setIndices(renderElem.second.indices, renderElem.second.numQuads * 6);

			mCachedMeshes[meshIdx]->setMeshData(meshData);
			mCachedMaterials[meshIdx] = renderElem.second.material;

			meshIdx++;
		}
	}

	void GUIWidget::render(const CameraPtr& camera, DeferredRenderContextPtr& renderContext) const
	{
		// Mesh is re-created every frame. There might be a better approach that only recreates it upon change,
		// but for now it seems like too much hassle for something like GUI that is pretty dynamic anyway.
		updateMeshes();

		// Render the meshes
		UINT32 meshIdx = 0;
		for(auto& mesh : mCachedMeshes)
		{
			MaterialHandle material = mCachedMaterials[meshIdx];

			if(material == nullptr || !material.isLoaded())
				continue;

			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			// TODO - Set current viewport resolution so that GUI can be rendered properly
			for(UINT32 i = 0; i < material->getNumPasses(); i++)
			{
				PassPtr pass = material->getPass(i);
				pass->activate(renderContext);

				PassParametersPtr paramsPtr = material->getPassParameters(i);
				pass->bindParameters(renderContext, paramsPtr);

				renderContext->render(mesh->getRenderOperation());
			}

			meshIdx++;
		}
	}
}