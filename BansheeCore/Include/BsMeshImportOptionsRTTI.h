#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsMeshImportOptions.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT MeshImportOptionsRTTI : public RTTIType < MeshImportOptions, ImportOptions, MeshImportOptionsRTTI >
	{
	private:
		bool& getCPUReadable(MeshImportOptions* obj) { return obj->mCPUReadable; }
		void setCPUReadable(MeshImportOptions* obj, bool& value) { obj->mCPUReadable = value; }
		
		bool& getImportNormals(MeshImportOptions* obj) { return obj->mImportNormals; }
		void setImportNormals(MeshImportOptions* obj, bool& value) { obj->mImportNormals = value; }
		
		bool& getImportTangents(MeshImportOptions* obj) { return obj->mImportTangents; }
		void setImportTangents(MeshImportOptions* obj, bool& value) { obj->mImportTangents = value; }
		
		bool& getImportBlendShapes(MeshImportOptions* obj) { return obj->mImportBlendShapes; }
		void setImportBlendShapes(MeshImportOptions* obj, bool& value) { obj->mImportBlendShapes = value; }
		
		bool& getImportSkin(MeshImportOptions* obj) { return obj->mImportSkin; }
		void setImportSkin(MeshImportOptions* obj, bool& value) { obj->mImportSkin = value; }
		
		bool& getImportAnimation(MeshImportOptions* obj) { return obj->mImportAnimation; }
		void setImportAnimation(MeshImportOptions* obj, bool& value) { obj->mImportAnimation = value; }

		float& getImportScale(MeshImportOptions* obj) { return obj->mImportScale; }
		void setImportScale(MeshImportOptions* obj, float& value) { obj->mImportScale = value; }

	public:
		MeshImportOptionsRTTI()
		{
			addPlainField("mCPUReadable", 0, &MeshImportOptionsRTTI::getCPUReadable, &MeshImportOptionsRTTI::setCPUReadable);
			addPlainField("mImportNormals", 1, &MeshImportOptionsRTTI::getImportNormals, &MeshImportOptionsRTTI::setImportNormals);
			addPlainField("mImportTangents", 2, &MeshImportOptionsRTTI::getImportTangents, &MeshImportOptionsRTTI::setImportTangents);
			addPlainField("mImportBlendShapes", 3, &MeshImportOptionsRTTI::getImportBlendShapes, &MeshImportOptionsRTTI::setImportBlendShapes);
			addPlainField("mImportSkin", 4, &MeshImportOptionsRTTI::getImportSkin, &MeshImportOptionsRTTI::setImportSkin);
			addPlainField("mImportAnimation", 5, &MeshImportOptionsRTTI::getImportAnimation, &MeshImportOptionsRTTI::setImportAnimation);
			addPlainField("mImportScale", 6, &MeshImportOptionsRTTI::getImportScale, &MeshImportOptionsRTTI::setImportScale);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "MeshImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_MeshImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<MeshImportOptions>();
		}
	};
}