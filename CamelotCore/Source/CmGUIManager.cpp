#include "CmGUIManager.h"
#include "CmGUIWidget.h"
#include "CmGUIElement.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"

namespace CamelotEngine
{
	struct MeshGroupID
	{
		class HashFunction
		{
		public:
			inline size_t operator()(const MeshGroupID& v) const
			{
				size_t seed = 0;
				hash_combine(seed, v.materialId);
				hash_combine(seed, v.meshGroupId);
				return seed;
			}
		};

		class EqualFunction
		{
		public:
			inline bool operator()(const MeshGroupID &a, const MeshGroupID &b) const
			{
				return a.materialId == b.materialId && a.meshGroupId == b.meshGroupId;
			}
		};

		MeshGroupID(UINT32 _meshGroupId, UINT64 _materialId)
			:meshGroupId(_meshGroupId), materialId(_materialId)
		{

		}

		UINT32 meshGroupId;
		UINT64 materialId;
		
	};

	vector<RenderObject>::type GUIManager::getRenderObjects()
	{
		updateDirtyMeshes();

		// TODO - It might be better to use standard Component system for rendering meshes
		// (i.e. make GUIManager a component and have it create "Renderable" components that have
		// meshes attached to them)
		UINT32 meshIdx = 0;
		vector<RenderObject>::type renderObjects;
		for(auto& mesh : mMeshes)
		{
			MaterialHandle material = mMaterials[meshIdx];

			if(material == nullptr || !material.isLoaded())
				continue;

			MeshHandle mesh = mMeshes[meshIdx];

			if(mesh == nullptr || !mesh.isLoaded())
				continue;

			// TODO - Set current viewport resolution so that GUI can be rendered properly
			//  - HOW DO I handle multiple GUIWidgets across different windows?

			RenderObject ro;
			ro.mat = material;
			ro.op = mesh->getRenderOperation(0);

			renderObjects.push_back(ro);

			meshIdx++;
		}

		return renderObjects;
	}

	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(widget);
	}

	void GUIManager::unregisterWidget(GUIWidget* widget)
	{
		auto findIter = std::find(begin(mWidgets), end(mWidgets), widget);

		if(findIter != end(mWidgets))
			mWidgets.erase(findIter);
	}

	void GUIManager::updateDirtyMeshes()
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

		std::unordered_map<MeshGroupID, TempMeshData, MeshGroupID::HashFunction, MeshGroupID::EqualFunction> meshDataPerRenderElement;

		// Group meshes based on widget mesh ID and used materials
		// Determine mesh sizes per group
		for(auto& widget : mWidgets)
		{
			for(auto& elem : widget->mElements)
			{
				UINT32 numRenderElems = elem->getNumRenderElements();

				for(UINT32 i = 0; i < numRenderElems; i++)
				{
					const MaterialHandle& mat = elem->getMaterial(i);

					MeshGroupID meshGroup(widget->mMeshGroupID, mat->getInternalID()); // TODO - I group based on material ID. So if two widgets used exact copies of the same material
																					   // this system won't detect it. Find a better way of determining material similarity?
																					   

					UINT32 numQuads = elem->getNumQuads(i);
					meshDataPerRenderElement[meshGroup].numQuads += numQuads;
					meshDataPerRenderElement[meshGroup].material = mat;
				}
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
		for(auto& widget : mWidgets)
		{
			for(auto& elem : widget->mElements)
			{
				UINT32 numRenderElems = elem->getNumRenderElements();

				for(UINT32 i = 0; i < numRenderElems; i++)
				{
					const MaterialHandle& mat = elem->getMaterial(i);
					MeshGroupID meshGroup(widget->mMeshGroupID, mat->getInternalID()); 

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
		}

		// Update meshes
		for(UINT32 i = (UINT32)mMeshes.size(); i < numMeshes; i++)
		{
			MeshHandle newMesh = Mesh::create();
			mMeshes.push_back(newMesh);
		}

		while((UINT32)mMeshes.size() > numMeshes && (UINT32)mMeshes.size() > 0)
		{
			mMeshes.erase(mMeshes.end() - 1); // TODO: Destroying meshes as soon as they're not used might be a perf. penalty?
											  //  Maybe instead pool the meshes and only actually remove them when a certain number of unused ones exists.
		}

		mMaterials.resize(numMeshes);

		UINT32 meshIdx = 0;
		for(auto& renderElem : meshDataPerRenderElement)
		{
			std::shared_ptr<MeshData> meshData(new MeshData());

			meshData->setPositions(renderElem.second.vertices, renderElem.second.numQuads * 4);
			meshData->setUV0(renderElem.second.uvs, renderElem.second.numQuads * 4);
			meshData->setIndices(renderElem.second.indices, renderElem.second.numQuads * 6);

			mMeshes[meshIdx]->setMeshData(meshData);
			mMaterials[meshIdx] = renderElem.second.material;

			meshIdx++;
		}
	}
}