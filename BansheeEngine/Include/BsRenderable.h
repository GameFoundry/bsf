#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsRenderableProxy.h"
#include "BsAABox.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	class BS_EXPORT Renderable : public Component
	{
		struct MeshData
		{
			MeshData() {}
			MeshData(const HMesh& mesh);

			HMesh mesh;
			mutable bool isLoaded;
		};

		struct MaterialData
		{
			MaterialData() {}
			MaterialData(const HMaterial& material);

			HMaterial material;
			mutable bool isLoaded;
		};

	public:
		void setMesh(HMesh mesh);
		void setNumMaterials(UINT32 numMaterials);
		void setMaterial(UINT32 idx, HMaterial material);
		void setMaterial(HMaterial material);
		void setLayer(UINT64 layer);

		UINT64 getLayer() const { return mLayer; }
		UINT32 getNumMaterials() const { return (UINT32)mMaterialData.size(); }
		HMaterial& getMaterial(UINT32 idx) { return mMaterialData[idx].material; }

		bool _isRenderDataDirty() const;
		void _markRenderDataClean() { mIsRenderDataDirty = false; }
		RenderableProxy* _createProxy(FrameAlloc* allocator) const;
		RenderableProxy* _getActiveProxy() const { return mActiveProxy; }
		void _setActiveProxy(RenderableProxy* proxy) { mActiveProxy = proxy; }

	private:
		void updateResourceLoadStates() const;

	private:
		MeshData mMeshData;
		Vector<MaterialData> mMaterialData;
		UINT64 mLayer;
		Vector<AABox> mWorldBounds;

		RenderableProxy* mActiveProxy;
		mutable bool mIsRenderDataDirty;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		/** Standard constructor.
        */
		Renderable(const HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Renderable() {} // Serialization only
	};
}