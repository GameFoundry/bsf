#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPass.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		BlendStatePtr getBlendState(Pass* obj) { return obj->mData.blendState; }
		void setBlendState(Pass* obj, BlendStatePtr val) { obj->mData.blendState = val; }

		RasterizerStatePtr getRasterizerState(Pass* obj) { return obj->mData.rasterizerState; }
		void setRasterizerState(Pass* obj, RasterizerStatePtr val) { obj->mData.rasterizerState = val; }

		DepthStencilStatePtr getDepthStencilState(Pass* obj) { return obj->mData.depthStencilState; }
		void setDepthStencilState(Pass* obj, DepthStencilStatePtr val) { obj->mData.depthStencilState = val; }

		GpuProgramPtr getVertexProgram(Pass* obj) { return obj->mData.vertexProgram; }
		void setVertexProgram(Pass* obj, GpuProgramPtr val) { obj->mData.vertexProgram = val; }

		GpuProgramPtr getFragmentProgram(Pass* obj) { return obj->mData.fragmentProgram; }
		void setFragmentProgram(Pass* obj, GpuProgramPtr val) { obj->mData.fragmentProgram = val; }

		GpuProgramPtr getGeometryProgram(Pass* obj) { return obj->mData.geometryProgram; }
		void setGeometryProgram(Pass* obj, GpuProgramPtr val) { obj->mData.geometryProgram = val; }

		GpuProgramPtr getHullProgram(Pass* obj) { return obj->mData.hullProgram; }
		void setHullProgram(Pass* obj, GpuProgramPtr val) { obj->mData.hullProgram = val; }

		GpuProgramPtr getDomainProgram(Pass* obj) { return obj->mData.domainProgram; }
		void setDomainProgram(Pass* obj, GpuProgramPtr val) { obj->mData.domainProgram = val; }

		GpuProgramPtr getComputeProgram(Pass* obj) { return obj->mData.computeProgram; }
		void setComputeProgram(Pass* obj, GpuProgramPtr val) { obj->mData.computeProgram = val; }
	public:
		PassRTTI()
		{
			addReflectablePtrField("mBlendState", 0, &PassRTTI::getBlendState, &PassRTTI::setBlendState);
			addReflectablePtrField("mRasterizerState", 1, &PassRTTI::getRasterizerState, &PassRTTI::setRasterizerState);
			addReflectablePtrField("mDepthStencilState", 2, &PassRTTI::getDepthStencilState, &PassRTTI::setDepthStencilState);

			addReflectablePtrField("mVertexProgram", 3, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectablePtrField("mFragmentProgram", 4, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectablePtrField("mGeometryProgram", 5, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
			addReflectablePtrField("mHullProgram", 6, &PassRTTI::getHullProgram, &PassRTTI::setHullProgram);
			addReflectablePtrField("mDomainProgram", 7, &PassRTTI::getDomainProgram, &PassRTTI::setDomainProgram);
			addReflectablePtrField("mComputeProgram", 8, &PassRTTI::getComputeProgram, &PassRTTI::setComputeProgram);
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