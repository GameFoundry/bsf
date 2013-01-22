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
		Material();

		/**
		 * @brief	Overridden from Resource.
		 */
		virtual void initialize_internal();

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
		void setSamplerState(const String& name, SamplerStatePtr samplerState);
		void setFloat(const String& name, float value);
		void setColor(const String& name, const Color& value);
		void setVec2(const String& name, const Vector2& value);
		void setVec3(const String& name, const Vector3& value);
		void setVec4(const String& name, const Vector4& value);
		void setMat3(const String& name, const Matrix3& value);
		void setMat4(const String& name, const Matrix4& value);

		void setParamBlock(const String& name, GpuParamBlockPtr paramBlock);

		UINT32 getNumPasses() const;

		PassPtr getPass(UINT32 passIdx) const;

		PassParametersPtr getPassParameters(UINT32 passIdx) const;

	private:
		ShaderPtr mShader;
		TechniquePtr mBestTechnique;

		set<String>::type mValidShareableParamBlocks;
		set<String>::type mValidParams;

		vector<PassParametersPtr>::type mParametersPerPass;

		void throwIfNotInitialized() const;

		template <typename T>
		void setParam(const String& name, T& value)
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
							paramPtr->setParam(name, value);
					}
				}
			}
		}

		void initBestTechnique();

		set<String>::type determineValidParameters(const vector<const GpuParamDesc*>::type& paramDescs) const;
		set<String>::type determineValidShareableParamBlocks(const vector<const GpuParamDesc*>::type& paramDescs) const;
		map<String, String>::type determineParameterToBlockMapping(const vector<const GpuParamDesc*>::type& paramDescs);

		bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets = false) const;
		bool areParamsEqual(const GpuParamObjectDesc& paramA, const GpuParamObjectDesc& paramB) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}