#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmCommonEnums.h"

namespace CamelotFramework
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
		struct StructData
		{
			StructData()
				:size(0), data(nullptr)
			{ }

			StructData(void* _data, UINT32 _size)
				:size(_size)
			{
				data = std::shared_ptr<void>(cm_alloc<ScratchAlloc>(_size), &cm_free<ScratchAlloc>);
				memcpy(data.get(), _data, size);
			}

			std::shared_ptr<void> data;
			UINT32 size;
		};

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

		void setTexture(const String& name, const HTexture& value);
		void setSamplerState(const String& name, const HSamplerState& samplerState);
		void setFloat(const String& name, float value, UINT32 arrayIdx = 0);
		void setColor(const String& name, const Color& value, UINT32 arrayIdx = 0);
		void setVec2(const String& name, const Vector2& value, UINT32 arrayIdx = 0);
		void setVec3(const String& name, const Vector3& value, UINT32 arrayIdx = 0);
		void setVec4(const String& name, const Vector4& value, UINT32 arrayIdx = 0);
		void setMat3(const String& name, const Matrix3& value, UINT32 arrayIdx = 0);
		void setMat4(const String& name, const Matrix4& value, UINT32 arrayIdx = 0);
		void setStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx = 0);
		void setRenderQueue(INT16 renderQueue) { mRenderQueue = renderQueue; }

		//void setParamBlock(const String& name, GpuParamBlockPtr paramBlock);

		HTexture getTexture(const String& name) const;
		HSamplerState getSamplerState(const String& name) const;
		float getFloat(const String& name, UINT32 arrayIdx = 0) const;
		Vector2 getVec2(const String& name, UINT32 arrayIdx = 0) const;
		Vector3 getVec3(const String& name, UINT32 arrayIdx = 0) const;
		Vector4 getVec4(const String& name, UINT32 arrayIdx = 0) const;
		Matrix3 getMat3(const String& name, UINT32 arrayIdx = 0) const;
		Matrix4 getMat4(const String& name, UINT32 arrayIdx = 0) const;
		const StructData& getStructData(const String& name, UINT32 arrayIdx = 0) const;
		INT16 getRenderQueue() const { return mRenderQueue; }

		UINT32 getNumPasses() const;

		PassPtr getPass(UINT32 passIdx) const;

		PassParametersPtr getPassParameters(UINT32 passIdx) const;

		static HMaterial create();
		static HMaterial create(ShaderPtr shader);
	protected:
		void destroy_internal();

	private:
		friend class MaterialManager;

		ShaderPtr mShader;
		TechniquePtr mBestTechnique;
		INT16 mRenderQueue;

		Set<String>::type mValidShareableParamBlocks;
		Map<String, String>::type mValidParams; // Also maps Shader param name -> gpu variable name

		Vector<PassParametersPtr>::type mParametersPerPass;
		Vector<GpuParamBlockBufferPtr>::type mParamBuffers;

		// These maps aren't necessary as we can read these values from the GpuParams directly
		// but they make many things (especially serializing and getting values) so much easier
		Map<String, Vector<float>::type>::type mFloatValues;
		Map<String, Vector<Vector2>::type>::type mVec2Values;
		Map<String, Vector<Vector3>::type>::type mVec3Values;
		Map<String, Vector<Vector4>::type>::type mVec4Values;
		Map<String, Vector<Matrix3>::type>::type mMat3Values;
		Map<String, Vector<Matrix4>::type>::type mMat4Values;
		Map<String, Vector<StructData>::type>::type mStructValues;
		Map<String, HTexture>::type mTextureValues;
		Map<String, HSamplerState>::type mSamplerValues;

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

		const Map<String, String>::type& getValidParamNames() const { return mValidParams; }

		void initBestTechnique();

		Map<String, const GpuParamDataDesc*>::type determineValidDataParameters(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Set<String>::type determineValidObjectParameters(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Set<String>::type determineValidShareableParamBlocks(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Map<String, String>::type determineParameterToBlockMapping(const Vector<const GpuParamDesc*>::type& paramDescs);

		bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets = false) const;

		void freeParamBuffers();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}