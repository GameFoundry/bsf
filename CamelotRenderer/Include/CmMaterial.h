#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmGpuParams.h"

namespace CamelotEngine
{
	class CM_EXPORT PassParameters
	{
	public:
		GpuParamsPtr mVertParams;
		GpuParamsPtr mFragParams;
		GpuParamsPtr mGeomParams;
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

		UINT32 getNumPasses() const;

		PassPtr getPass(UINT32 passIdx) const;

		PassParametersPtr getPassParameters(UINT32 passIdx) const;

	private:
		ShaderPtr mShader;
		TechniquePtr mBestTechnique;
		vector<PassParametersPtr>::type mParameters;

		void throwIfNotInitialized() const;

		template <typename T>
		void setParam(const String& name, T& value)
		{
			for(auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
			{
				PassParametersPtr params = *iter;

				if(params->mVertParams)
				{
					if(params->mVertParams->hasParam(name))
						params->mVertParams->setParam(name, value);
				}

				if(params->mFragParams)
				{
					if(params->mFragParams->hasParam(name))
						params->mFragParams->setParam(name, value);
				}

				if(params->mGeomParams)
				{
					if(params->mGeomParams->hasParam(name))
						params->mGeomParams->setParam(name, value);
				}
			}
		}

		void initBestTechnique();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}