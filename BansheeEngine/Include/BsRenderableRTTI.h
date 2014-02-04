#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsRenderable.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableRTTI : public CM::RTTIType<Renderable, CM::Component, RenderableRTTI>
	{
	private:
		CM::HMesh& getMesh(Renderable* obj) { return obj->mMesh; }
		void setMesh(Renderable* obj, CM::HMesh& val) { obj->mMesh = val; } 

		CM::UINT64& getLayer(Renderable* obj) { return obj->mLayer; }
		void setLayer(Renderable* obj, CM::UINT64& val) { obj->mLayer = val; }

		CM::HMaterial& getMaterial(Renderable* obj, CM::UINT32 idx) { return obj->mMaterials[idx]; }
		void setMaterial(Renderable* obj, CM::UINT32 idx, CM::HMaterial& val) { obj->setMaterial(idx, val); }
		CM::UINT32 getNumMaterials(Renderable* obj) { return (CM::UINT32)obj->mMaterials.size(); }
		void setNumMaterials(Renderable* obj, CM::UINT32 num) { obj->setNumMaterials(num); }

	public:
		RenderableRTTI()
		{
			addReflectableField("mMesh", 0, &RenderableRTTI::getMesh, &RenderableRTTI::setMesh);
			addPlainField("mLayer", 1, &RenderableRTTI::getLayer, &RenderableRTTI::setLayer);
			addReflectableArrayField("mMaterials", 2, &RenderableRTTI::getMaterial, &RenderableRTTI::getNumMaterials, &RenderableRTTI::setMaterial, &RenderableRTTI::setNumMaterials);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "Renderable";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return TID_Renderable;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<Renderable, CM::PoolAlloc>(new (CM::cm_alloc<Renderable, CM::PoolAlloc>()) Renderable());
		}
	};
}