#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsRenderableHandler.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableHandlerRTTI : public RTTIType<RenderableHandler, IReflectable, RenderableHandlerRTTI>
	{
	private:
		HMesh& getMesh(RenderableHandler* obj) { return obj->mMesh; }
		void setMesh(RenderableHandler* obj, HMesh& val) { obj->mMesh = val; }

		UINT64& getLayer(RenderableHandler* obj) { return obj->mLayer; }
		void setLayer(RenderableHandler* obj, UINT64& val) { obj->mLayer = val; }

		HMaterial& getMaterial(RenderableHandler* obj, UINT32 idx) { return obj->mMaterials[idx]; }
		void setMaterial(RenderableHandler* obj, UINT32 idx, HMaterial& val) { obj->setMaterial(idx, val); }
		UINT32 getNumMaterials(RenderableHandler* obj) { return (UINT32)obj->mMaterials.size(); }
		void setNumMaterials(RenderableHandler* obj, UINT32 num) { obj->mMaterials.resize(num); }

	public:
		RenderableHandlerRTTI()
		{
			addReflectableField("mMesh", 0, &RenderableHandlerRTTI::getMesh, &RenderableHandlerRTTI::setMesh);
			addPlainField("mLayer", 1, &RenderableHandlerRTTI::getLayer, &RenderableHandlerRTTI::setLayer);
			addReflectableArrayField("mMaterials", 2, &RenderableHandlerRTTI::getMaterial, 
				&RenderableHandlerRTTI::getNumMaterials, &RenderableHandlerRTTI::setMaterial, &RenderableHandlerRTTI::setNumMaterials);
		}

		virtual void onDeserializationEnded(IReflectable* obj) override
		{
			// Note: Since this is a CoreObject I should call initialize() right after deserialization,
			// but since this specific type is used in Components we delay initialization until Component
			// itself does it. Keep this is mind in case this ever needs to be deserialized for non-Component 
			// purposes (you'll need to call initialize manually).
		}

		virtual const String& getRTTIName() override
		{
			static String name = "RenderableHandler";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_RenderableHandler;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return RenderableHandler::createEmpty();
		}
	};
}