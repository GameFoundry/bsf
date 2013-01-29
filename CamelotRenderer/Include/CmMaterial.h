#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	class CM_EXPORT PassParameters
	{
	public:
		GpuParamsPtr mVertParams;
		GpuParamsPtr mFragParams;
		GpuParamsPtr mGeomParams;
		GpuParamsPtr mHullParams;
		GpuParamsPtr mDomainParams;
		GpuParamsPtr mComputeParams;

		// Helper method
		GpuParamsPtr& getParamByIdx(UINT32 idx)
		{
			GpuParamsPtr* paramArray[] = {&mVertParams, &mFragParams, &mGeomParams, &mHullParams, &mDomainParams, &mComputeParams};

			return *paramArray[idx];
		}

		// Helper method
		UINT32 getNumParams() const { return 6; }
	};

	class CM_EXPORT Material : public Resource
	{
	public:
		~Material();

		/**
		 * @brief	Sets a shader that will be used by the material. 
		 * 			Shaders best technique will be retrieved and used in all subsequent Material
		 * 			operations. 
		 * 			You need to call this method before doing any other operations with this class.
		 * 			After setting the shader if change any systems that a shader technique is defendant upon (render system, renderer, ...)
		 * 			you will need to call this method again on all your Materials to make sure technique used is updated.
		 */
		void setShader(ShaderPtr shader);

		ShaderPtr getShader() const { return mShader; }

		void setTexture(const String& name, TextureHandle& value);
		void setSamplerState(const String& name, SamplerStateHandle& samplerState);
		void setFloat(const String& name, float value, UINT32 arrayIdx = 0);
		void setColor(const String& name, const Color& value, UINT32 arrayIdx = 0);
		void setVec2(const String& name, const Vector2& value, UINT32 arrayIdx = 0);
		void setVec3(const String& name, const Vector3& value, UINT32 arrayIdx = 0);
		void setVec4(const String& name, const Vector4& value, UINT32 arrayIdx = 0);
		void setMat3(const String& name, const Matrix3& value, UINT32 arrayIdx = 0);
		void setMat4(const String& name, const Matrix4& value, UINT32 arrayIdx = 0);

		void setParamBlock(const String& name, GpuParamBlockPtr paramBlock);

		UINT32 getNumPasses() const;

		PassPtr getPass(UINT32 passIdx) const;

		PassParametersPtr getPassParameters(UINT32 passIdx) const;

		static MaterialPtr create();
	private:
		ShaderPtr mShader;
		TechniquePtr mBestTechnique;

		set<String>::type mValidShareableParamBlocks;
		map<String, String>::type mValidParams; // Also maps Shader param name -> gpu variable name

		vector<PassParametersPtr>::type mParametersPerPass;

		// These maps aren't necessary as we can read these values from the GpuParams directly
		// but they make many things (especially serializing and getting values) so much easier
		map<String, vector<float>::type>::type mFloatValues;
		map<String, vector<Vector2>::type>::type mVec2Values;
		map<String, vector<Vector3>::type>::type mVec3Values;
		map<String, vector<Vector4>::type>::type mVec4Values;
		map<String, vector<Matrix3>::type>::type mMat3Values;
		map<String, vector<Matrix4>::type>::type mMat4Values;
		map<String, TextureHandle>::type mTextureValues;
		map<String, SamplerStateHandle>::type mSamplerValues;

		Material();

		void throwIfNotInitialized() const;

		template <typename T>
		void setParam(const String& name, T& value, UINT32 arrayIdx)
		{
			for(auto iter = mParametersPerPass.begin(); iter != mParametersPerPass.end(); ++iter)
			{
				PassParametersPtr params = *iter;

				for(UINT32 i = 0; i < params->getNumParams(); i++)
				{
					GpuParamsPtr& paramPtr = params->getParamByIdx(i);
					if(paramPtr)
					{
						if(paramPtr->hasParam(name))
							paramPtr->setParam(name, value, arrayIdx);
					}
				}
			}
		}

		const map<String, String>::type& getValidParamNames() const { return mValidParams; }

		TextureHandle getTexture(const String& name) const;
		SamplerStateHandle getSamplerState(const String& name) const;
		float getFloat(const String& name, UINT32 arrayIdx = 0) const;
		Vector2 getVec2(const String& name, UINT32 arrayIdx = 0) const;
		Vector3 getVec3(const String& name, UINT32 arrayIdx = 0) const;
		Vector4 getVec4(const String& name, UINT32 arrayIdx = 0) const;
		Matrix3 getMat3(const String& name, UINT32 arrayIdx = 0) const;
		Matrix4 getMat4(const String& name, UINT32 arrayIdx = 0) const;

		void initBestTechnique();

		map<String, const GpuParamDataDesc*>::type determineValidDataParameters(const vector<const GpuParamDesc*>::type& paramDescs) const;
		set<String>::type determineValidObjectParameters(const vector<const GpuParamDesc*>::type& paramDescs) const;
		set<String>::type determineValidShareableParamBlocks(const vector<const GpuParamDesc*>::type& paramDescs) const;
		map<String, String>::type determineParameterToBlockMapping(const vector<const GpuParamDesc*>::type& paramDescs);

		bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets = false) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}