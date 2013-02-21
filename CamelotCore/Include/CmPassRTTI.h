#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmPass.h"

namespace CamelotEngine
{
	class CM_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		BlendStateHandle& getBlendState(Pass* obj) { return obj->mBlendState; }
		void setBlendState(Pass* obj, BlendStateHandle& val) { obj->mBlendState = val; } 

		RasterizerStateHandle& getRasterizerState(Pass* obj) { return obj->mRasterizerState; }
		void setRasterizerState(Pass* obj, RasterizerStateHandle& val) { obj->mRasterizerState = val; } 

		DepthStencilStateHandle& getDepthStencilState(Pass* obj) { return obj->mDepthStencilState; }
		void setDepthStencilState(Pass* obj, DepthStencilStateHandle& val) { obj->mDepthStencilState = val; } 

		GpuProgramHandle& getVertexProgram(Pass* obj) { return obj->mVertexProgram; }
		void setVertexProgram(Pass* obj, GpuProgramHandle& val) { obj->mVertexProgram = val; } 

		GpuProgramHandle& getFragmentProgram(Pass* obj) { return obj->mFragmentProgram; }
		void setFragmentProgram(Pass* obj, GpuProgramHandle& val) { obj->mFragmentProgram = val; } 

		GpuProgramHandle& getGeometryProgram(Pass* obj) { return obj->mGeometryProgram; }
		void setGeometryProgram(Pass* obj, GpuProgramHandle& val) { obj->mGeometryProgram = val; } 

		GpuProgramHandle& getHullProgram(Pass* obj) { return obj->mHullProgram; }
		void setHullProgram(Pass* obj, GpuProgramHandle& val) { obj->mHullProgram = val; } 

		GpuProgramHandle& getDomainProgram(Pass* obj) { return obj->mDomainProgram; }
		void setDomainProgram(Pass* obj, GpuProgramHandle& val) { obj->mDomainProgram = val; } 

		GpuProgramHandle& getComputeProgram(Pass* obj) { return obj->mComputeProgram; }
		void setComputeProgram(Pass* obj, GpuProgramHandle& val) { obj->mComputeProgram = val; } 
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
			return std::shared_ptr<Pass>(new Pass());
		}
	};
}