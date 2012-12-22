#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmPass.h"

namespace CamelotEngine
{
	class CM_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		BlendStatePtr getBlendState(Pass* obj) { return obj->mBlendState; }
		void setBlendState(Pass* obj, BlendStatePtr val) { obj->mBlendState = val; } 

		RasterizerStatePtr getRasterizerState(Pass* obj) { return obj->mRasterizerState; }
		void setRasterizerState(Pass* obj, RasterizerStatePtr val) { obj->mRasterizerState = val; } 

		DepthStencilStatePtr getDepthStencilState(Pass* obj) { return obj->mDepthStencilState; }
		void setDepthStencilState(Pass* obj, DepthStencilStatePtr val) { obj->mDepthStencilState = val; } 

		GpuProgramHandle& getVertexProgram(Pass* obj) { return obj->mVertexProgram; }
		void setVertexProgram(Pass* obj, GpuProgramHandle& val) { obj->mVertexProgram = val; } 

		GpuProgramHandle& getFragmentProgram(Pass* obj) { return obj->mFragmentProgram; }
		void setFragmentProgram(Pass* obj, GpuProgramHandle& val) { obj->mFragmentProgram = val; } 

		GpuProgramHandle& getGeometryProgram(Pass* obj) { return obj->mGeometryProgram; }
		void setGeometryProgram(Pass* obj, GpuProgramHandle& val) { obj->mGeometryProgram = val; } 
	public:
		PassRTTI()
		{
			addReflectablePtrField("mBlendState", 0, &PassRTTI::getBlendState, &PassRTTI::setBlendState);
			addReflectablePtrField("mRasterizerState", 1, &PassRTTI::getRasterizerState, &PassRTTI::setRasterizerState);
			addReflectablePtrField("mDepthStencilState", 2, &PassRTTI::getDepthStencilState, &PassRTTI::setDepthStencilState);

			addReflectableField("mVertexProgram", 3, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectableField("mFragmentProgram", 4, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectableField("mGeometryProgram", 5, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
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