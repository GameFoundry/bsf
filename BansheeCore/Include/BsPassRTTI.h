#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPass.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		HBlendState& getBlendState(Pass* obj) { return obj->mData.blendState; }
		void setBlendState(Pass* obj, HBlendState& val) { obj->mData.blendState = val; }

		HRasterizerState& getRasterizerState(Pass* obj) { return obj->mData.rasterizerState; }
		void setRasterizerState(Pass* obj, HRasterizerState& val) { obj->mData.rasterizerState = val; }

		HDepthStencilState& getDepthStencilState(Pass* obj) { return obj->mData.depthStencilState; }
		void setDepthStencilState(Pass* obj, HDepthStencilState& val) { obj->mData.depthStencilState = val; }

		HGpuProgram& getVertexProgram(Pass* obj) { return obj->mData.vertexProgram; }
		void setVertexProgram(Pass* obj, HGpuProgram& val) { obj->mData.vertexProgram = val; }

		HGpuProgram& getFragmentProgram(Pass* obj) { return obj->mData.fragmentProgram; }
		void setFragmentProgram(Pass* obj, HGpuProgram& val) { obj->mData.fragmentProgram = val; }

		HGpuProgram& getGeometryProgram(Pass* obj) { return obj->mData.geometryProgram; }
		void setGeometryProgram(Pass* obj, HGpuProgram& val) { obj->mData.geometryProgram = val; }

		HGpuProgram& getHullProgram(Pass* obj) { return obj->mData.hullProgram; }
		void setHullProgram(Pass* obj, HGpuProgram& val) { obj->mData.hullProgram = val; }

		HGpuProgram& getDomainProgram(Pass* obj) { return obj->mData.domainProgram; }
		void setDomainProgram(Pass* obj, HGpuProgram& val) { obj->mData.domainProgram = val; }

		HGpuProgram& getComputeProgram(Pass* obj) { return obj->mData.computeProgram; }
		void setComputeProgram(Pass* obj, HGpuProgram& val) { obj->mData.computeProgram = val; }
	public:
		PassRTTI()
		{
			addReflectableField("mBlendState", 0, &PassRTTI::getBlendState, &PassRTTI::setBlendState);
			addReflectableField("mRasterizerState", 1, &PassRTTI::getRasterizerState, &PassRTTI::setRasterizerState);
			addReflectableField("mDepthStencilState", 2, &PassRTTI::getDepthStencilState, &PassRTTI::setDepthStencilState);

			addReflectableField("mVertexProgram", 3, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectableField("mFragmentProgram", 4, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectableField("mGeometryProgram", 5, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
			addReflectableField("mHullProgram", 6, &PassRTTI::getHullProgram, &PassRTTI::setHullProgram);
			addReflectableField("mDomainProgram", 7, &PassRTTI::getDomainProgram, &PassRTTI::setDomainProgram);
			addReflectableField("mComputeProgram", 8, &PassRTTI::getComputeProgram, &PassRTTI::setComputeProgram);
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Pass* pass = static_cast<Pass*>(obj);
			pass->initialize();
		}

		virtual const String& getRTTIName()
		{
			static String name = "Pass";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Pass;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return Pass::createEmpty();
		}
	};
}