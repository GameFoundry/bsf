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

		bool& getDepthCheck(Pass* obj) { return obj->mDepthCheck; }
		void setDepthCheck(Pass* obj, bool& val) { obj->mDepthCheck = val; } 

		bool& getDepthWrite(Pass* obj) { return obj->mDepthWrite; }
		void setDepthWrite(Pass* obj, bool& val) { obj->mDepthWrite = val; } 

		CompareFunction& getCompareFunction(Pass* obj) { return obj->mDepthFunc; }
		void setCompareFunction(Pass* obj, CompareFunction& val) { obj->mDepthFunc = val; } 

		float& getDepthBiasConstant(Pass* obj) { return obj->mDepthBiasConstant; }
		void setDepthBiasConstant(Pass* obj, float& val) { obj->mDepthBiasConstant = val; } 

		float& getDepthBiasSlopeScale(Pass* obj) { return obj->mDepthBiasSlopeScale; }
		void setDepthBiasSlopeScale(Pass* obj, float& val) { obj->mDepthBiasSlopeScale = val; } 

		float& getDepthBiasPerIteration(Pass* obj) { return obj->mDepthBiasPerIteration; }
		void setDepthBiasPerIteration(Pass* obj, float& val) { obj->mDepthBiasPerIteration = val; } 

		CullingMode& getCullMode(Pass* obj) { return obj->mCullMode; }
		void setCullMode(Pass* obj, CullingMode& val) { obj->mCullMode = val; } 

		PolygonMode& getPolygonMode(Pass* obj) { return obj->mPolygonMode; }
		void setPolygonMode(Pass* obj, PolygonMode& val) { obj->mPolygonMode = val; } 

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

			addPlainField("mDepthCheck", 1, &PassRTTI::getDepthCheck, &PassRTTI::setDepthCheck);
			addPlainField("mDepthWrite", 2, &PassRTTI::getDepthWrite, &PassRTTI::setDepthWrite);
			addPlainField("mDepthFunc", 3, &PassRTTI::getCompareFunction, &PassRTTI::setCompareFunction);
			addPlainField("mDepthBiasConstant", 4, &PassRTTI::getDepthBiasConstant, &PassRTTI::setDepthBiasConstant);
			addPlainField("mDepthBiasSlopeScale", 5, &PassRTTI::getDepthBiasSlopeScale, &PassRTTI::setDepthBiasSlopeScale);
			addPlainField("mDepthBiasPerIteration", 6, &PassRTTI::getDepthBiasPerIteration, &PassRTTI::setDepthBiasPerIteration);

			addPlainField("mCullMode", 7, &PassRTTI::getCullMode, &PassRTTI::setCullMode);
			addPlainField("mPolygonMode", 8, &PassRTTI::getPolygonMode, &PassRTTI::setPolygonMode);

			addReflectableField("mVertexProgram", 9, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectableField("mFragmentProgram", 10, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectableField("mGeometryProgram", 11, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
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