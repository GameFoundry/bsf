#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmPass.h"

namespace CamelotEngine
{
	class CM_EXPORT PassRTTI : public RTTIType<Pass, IReflectable, PassRTTI>
	{
	private:
		SceneBlendFactor& getSourceBlendFactor(Pass* obj) { return obj->mSourceBlendFactor; }
		void setSourceBlendFactor(Pass* obj, SceneBlendFactor& val) { obj->mSourceBlendFactor = val; } 

		SceneBlendFactor& getDestBlendFactor(Pass* obj) { return obj->mDestBlendFactor; }
		void setDestBlendFactor(Pass* obj, SceneBlendFactor& val) { obj->mDestBlendFactor = val; } 

		SceneBlendFactor& getSourceBlendFactorAlpha(Pass* obj) { return obj->mSourceBlendFactorAlpha; }
		void setSourceBlendFactorAlpha(Pass* obj, SceneBlendFactor& val) { obj->mSourceBlendFactorAlpha = val; } 

		SceneBlendFactor& getDestBlendFactorAlpha(Pass* obj) { return obj->mDestBlendFactorAlpha; }
		void setDestBlendFactorAlpha(Pass* obj, SceneBlendFactor& val) { obj->mDestBlendFactorAlpha = val; } 

		bool& getSeparateBlend(Pass* obj) { return obj->mSeparateBlend; }
		void setSeparateBlend(Pass* obj, bool& val) { obj->mSeparateBlend = val; } 

		SceneBlendOperation& getBlendOperation(Pass* obj) { return obj->mBlendOperation; }
		void setBlendOperation(Pass* obj, SceneBlendOperation& val) { obj->mBlendOperation = val; } 

		SceneBlendOperation& getAlphaBlendOperation(Pass* obj) { return obj->mAlphaBlendOperation; }
		void setAlphaBlendOperation(Pass* obj, SceneBlendOperation& val) { obj->mAlphaBlendOperation = val; } 

		bool& getSeparateBlendOperation(Pass* obj) { return obj->mSeparateBlendOperation; }
		void setSeparateBlendOperation(Pass* obj, bool& val) { obj->mSeparateBlendOperation = val; } 

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

		bool& getColourWrite(Pass* obj) { return obj->mColourWrite; }
		void setColourWrite(Pass* obj, bool& val) { obj->mColourWrite = val; } 

		CompareFunction& getAlphaRejectFunc(Pass* obj) { return obj->mAlphaRejectFunc; }
		void setAlphaRejectFunc(Pass* obj, CompareFunction& val) { obj->mAlphaRejectFunc = val; } 

		UINT8& getAlphaRejectVal(Pass* obj) { return obj->mAlphaRejectVal; }
		void setAlphaRejectVal(Pass* obj, UINT8& val) { obj->mAlphaRejectVal = val; } 

		bool& getAlphaToCoverageEnabled(Pass* obj) { return obj->mAlphaToCoverageEnabled; }
		void setAlphaToCoverageEnabled(Pass* obj, bool& val) { obj->mAlphaToCoverageEnabled = val; } 

		CullingMode& getCullMode(Pass* obj) { return obj->mCullMode; }
		void setCullMode(Pass* obj, CullingMode& val) { obj->mCullMode = val; } 

		PolygonMode& getPolygonMode(Pass* obj) { return obj->mPolygonMode; }
		void setPolygonMode(Pass* obj, PolygonMode& val) { obj->mPolygonMode = val; } 

		float& getPointSize(Pass* obj) { return obj->mPointSize; }
		void setPointSize(Pass* obj, float& val) { obj->mPointSize = val; } 

		float& getPointMinSize(Pass* obj) { return obj->mPointMinSize; }
		void setPointMinSize(Pass* obj, float& val) { obj->mPointMinSize = val; } 

		float& getPointMaxSize(Pass* obj) { return obj->mPointMaxSize; }
		void setPointMaxSize(Pass* obj, float& val) { obj->mPointMaxSize = val; } 

		GpuProgramHandle& getVertexProgram(Pass* obj) { return obj->mVertexProgram; }
		void setVertexProgram(Pass* obj, GpuProgramHandle& val) { obj->mVertexProgram = val; } 

		GpuProgramHandle& getFragmentProgram(Pass* obj) { return obj->mFragmentProgram; }
		void setFragmentProgram(Pass* obj, GpuProgramHandle& val) { obj->mFragmentProgram = val; } 

		GpuProgramHandle& getGeometryProgram(Pass* obj) { return obj->mGeometryProgram; }
		void setGeometryProgram(Pass* obj, GpuProgramHandle& val) { obj->mGeometryProgram = val; } 
	public:
		PassRTTI()
		{
			addPlainField("mSourceBlendFactor", 0, &PassRTTI::getSourceBlendFactor, &PassRTTI::setSourceBlendFactor);
			addPlainField("mDestBlendFactor", 1, &PassRTTI::getDestBlendFactor, &PassRTTI::setDestBlendFactor);
			addPlainField("mSourceBlendFactorAlpha", 2, &PassRTTI::getSourceBlendFactorAlpha, &PassRTTI::setSourceBlendFactorAlpha);
			addPlainField("mDestBlendFactorAlpha", 3, &PassRTTI::getDestBlendFactorAlpha, &PassRTTI::setDestBlendFactorAlpha);

			addPlainField("mSeparateBlend", 4, &PassRTTI::getSeparateBlend, &PassRTTI::setSeparateBlend);
			addPlainField("mBlendOperation", 5, &PassRTTI::getBlendOperation, &PassRTTI::setBlendOperation);
			addPlainField("mAlphaBlendOperation", 6, &PassRTTI::getAlphaBlendOperation, &PassRTTI::setAlphaBlendOperation);
			addPlainField("mSeparateBlendOperation", 7, &PassRTTI::getSeparateBlendOperation, &PassRTTI::setSeparateBlendOperation);

			addPlainField("mDepthCheck", 8, &PassRTTI::getDepthCheck, &PassRTTI::setDepthCheck);
			addPlainField("mDepthWrite", 9, &PassRTTI::getDepthWrite, &PassRTTI::setDepthWrite);
			addPlainField("mDepthFunc", 10, &PassRTTI::getCompareFunction, &PassRTTI::setCompareFunction);
			addPlainField("mDepthBiasConstant", 11, &PassRTTI::getDepthBiasConstant, &PassRTTI::setDepthBiasConstant);
			addPlainField("mDepthBiasSlopeScale", 12, &PassRTTI::getDepthBiasSlopeScale, &PassRTTI::setDepthBiasSlopeScale);
			addPlainField("mDepthBiasPerIteration", 13, &PassRTTI::getDepthBiasPerIteration, &PassRTTI::setDepthBiasPerIteration);

			addPlainField("mColourWrite", 14, &PassRTTI::getColourWrite, &PassRTTI::setColourWrite);
			addPlainField("mAlphaRejectFunc", 15, &PassRTTI::getAlphaRejectFunc, &PassRTTI::setAlphaRejectFunc);
			addPlainField("mAlphaRejectVal", 16, &PassRTTI::getAlphaRejectVal, &PassRTTI::setAlphaRejectVal);
			addPlainField("mAlphaToCoverageEnabled", 17, &PassRTTI::getAlphaToCoverageEnabled, &PassRTTI::setAlphaToCoverageEnabled);

			addPlainField("mCullMode", 20, &PassRTTI::getCullMode, &PassRTTI::setCullMode);
			addPlainField("mPolygonMode", 21, &PassRTTI::getPolygonMode, &PassRTTI::setPolygonMode);

			addPlainField("mPointSize", 23, &PassRTTI::getPointSize, &PassRTTI::setPointSize);
			addPlainField("mPointMinSize", 24, &PassRTTI::getPointMinSize, &PassRTTI::setPointMinSize);
			addPlainField("mPointMaxSize", 25, &PassRTTI::getPointMaxSize, &PassRTTI::setPointMaxSize);

			addReflectableField("mVertexProgram", 26, &PassRTTI::getVertexProgram, &PassRTTI::setVertexProgram);
			addReflectableField("mFragmentProgram", 27, &PassRTTI::getFragmentProgram, &PassRTTI::setFragmentProgram);
			addReflectableField("mGeometryProgram", 28, &PassRTTI::getGeometryProgram, &PassRTTI::setGeometryProgram);
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