#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class Material
	{
		struct ParamsPerPass
		{
			GpuProgramParametersPtr mVertParams;
			GpuProgramParametersPtr mFragParams;
			GpuProgramParametersPtr mGeomParams;
		};

	public:
		/**
		 * @brief	Sets a shader that will be used by the material. 
		 * 			Shaders best technique will be retrieved and used in all subsequent Material
		 * 			operations. 
		 * 			You need to call this method before doing any other operations with this class.
		 * 			After setting the shader if change any systems that a shader technique is defendant upon (render system, renderer, ...)
		 * 			you will need to call this method again on all your Materials to make sure technique used is updated.
		 */
		void setShader(ShaderPtr shader);

		void setTexture(const String& name, TextureRef& value);
		void setFloat(const String& name, float value);
		void setColor(const String& name, const Color& value);
		void setVec2(const String& name, const Vector2& value);
		void setVec3(const String& name, const Vector3& value);
		void setVec4(const String& name, const Vector4& value);
		void setMat3(const String& name, const Matrix3& value);
		void setMat4(const String& name, const Matrix4& value);

		/**
		 * @brief	Use the provided pass for rendering of all following objects.
		 */
		void applyPass(UINT32 passIdx);

		UINT32 getNumPasses() const;

	private:
		ShaderPtr mShader;
		TechniquePtr mBestTechnique;
		vector<ParamsPerPass>::type mParameters;

		void throwIfNotInitialized() const;

		template <typename T>
		void setParam(const String& name, T& value)
		{
			for(auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
			{
				ParamsPerPass params = *iter;

				if(params.mVertParams)
				{
					if(params.mVertParams->hasNamedConstant(name))
						params.mVertParams->setNamedConstant(name, value);
				}

				if(params.mFragParams)
				{
					if(params.mFragParams->hasNamedConstant(name))
						params.mFragParams->setNamedConstant(name, value);
				}

				if(params.mGeomParams)
				{
					if(params.mGeomParams->hasNamedConstant(name))
						params.mGeomParams->setNamedConstant(name, value);
				}
			}
		}
	};
}