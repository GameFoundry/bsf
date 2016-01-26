#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsRenderable.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableRTTI : public RTTIType<Renderable, Component, RenderableRTTI>
	{
	private:
		HMesh& getMesh(Renderable* obj) { return obj->mMeshData.mesh; }
		void setMesh(Renderable* obj, HMesh& val) { obj->mMeshData.mesh = val; } 

		UINT64& getLayer(Renderable* obj) { return obj->mLayer; }
		void setLayer(Renderable* obj, UINT64& val) { obj->mLayer = val; }

		HMaterial& getMaterial(Renderable* obj, UINT32 idx) { return obj->mMaterialData[idx].material; }
		void setMaterial(Renderable* obj, UINT32 idx, HMaterial& val) { obj->setMaterial(idx, val); }
		UINT32 getNumMaterials(Renderable* obj) { return (UINT32)obj->mMaterialData.size(); }
		void setNumMaterials(Renderable* obj, UINT32 num) { obj->mMaterialData.resize(num); }

	public:
		RenderableRTTI()
		{
			addReflectableField("mMesh", 0, &RenderableRTTI::getMesh, &RenderableRTTI::setMesh);
			addPlainField("mLayer", 1, &RenderableRTTI::getLayer, &RenderableRTTI::setLayer);
			addReflectableArrayField("mMaterials", 2, &RenderableRTTI::getMaterial, &RenderableRTTI::getNumMaterials, &RenderableRTTI::setMaterial, &RenderableRTTI::setNumMaterials);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Renderable";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Renderable;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<Renderable>();
		}
	};
}