#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsRenderableHandler.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableHandlerRTTI : public RTTIType<RenderableHandler, IReflectable, RenderableHandlerRTTI>
	{
	private:
		HMesh& getMesh(RenderableHandler* obj) { return obj->mMeshData.mesh; }
		void setMesh(RenderableHandler* obj, HMesh& val) { obj->mMeshData.mesh = val; }

		UINT64& getLayer(RenderableHandler* obj) { return obj->mLayer; }
		void setLayer(RenderableHandler* obj, UINT64& val) { obj->mLayer = val; }

		HMaterial& getMaterial(RenderableHandler* obj, UINT32 idx) { return obj->mMaterialData[idx].material; }
		void setMaterial(RenderableHandler* obj, UINT32 idx, HMaterial& val) { obj->setMaterial(idx, val); }
		UINT32 getNumMaterials(RenderableHandler* obj) { return (UINT32)obj->mMaterialData.size(); }
		void setNumMaterials(RenderableHandler* obj, UINT32 num) { obj->mMaterialData.resize(num); }

	public:
		RenderableHandlerRTTI()
		{
			addReflectableField("mMesh", 0, &RenderableHandlerRTTI::getMesh, &RenderableHandlerRTTI::setMesh);
			addPlainField("mLayer", 1, &RenderableHandlerRTTI::getLayer, &RenderableHandlerRTTI::setLayer);
			addReflectableArrayField("mMaterials", 2, &RenderableHandlerRTTI::getMaterial, 
				&RenderableHandlerRTTI::getNumMaterials, &RenderableHandlerRTTI::setMaterial, &RenderableHandlerRTTI::setNumMaterials);
		}

		virtual const String& getRTTIName()
		{
			static String name = "RenderableHandler";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_RenderableHandler;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr<RenderableHandler>();
		}
	};
}