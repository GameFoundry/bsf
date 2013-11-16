#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmGpuParam.h"
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

			StructData(UINT32 _size)
				:size(_size)
			{
				data = std::shared_ptr<void>(cm_alloc<ScratchAlloc>(_size), &cm_free<ScratchAlloc>);
			}

			void write(void* _data)
			{
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

		void setTexture(const String& name, const HTexture& value)								{ return getParamTexture(name).set(value); }
		void setSamplerState(const String& name, const HSamplerState& value)					{ return getParamSamplerState(name).set(value); }
		void setFloat(const String& name, float value, UINT32 arrayIdx = 0)						{ return getParamFloat(name).set(value, arrayIdx); }
		void setColor(const String& name, const Color& value, UINT32 arrayIdx = 0);
		void setVec2(const String& name, const Vector2& value, UINT32 arrayIdx = 0)				{ return getParamVec2(name).set(value, arrayIdx); }
		void setVec3(const String& name, const Vector3& value, UINT32 arrayIdx = 0)				{ return getParamVec3(name).set(value, arrayIdx); }
		void setVec4(const String& name, const Vector4& value, UINT32 arrayIdx = 0)				{ return getParamVec4(name).set(value, arrayIdx); }
		void setMat3(const String& name, const Matrix3& value, UINT32 arrayIdx = 0)				{ return getParamMat3(name).set(value, arrayIdx); }
		void setMat4(const String& name, const Matrix4& value, UINT32 arrayIdx = 0)				{ return getParamMat4(name).set(value, arrayIdx); }
		void setStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx = 0)	{ return getParamStruct(name).set(value, size, arrayIdx); }
		void setRenderQueue(INT16 renderQueue) { mRenderQueue = renderQueue; }

		//void setParamBlock(const String& name, GpuParamBlockPtr paramBlock);

		HTexture getTexture(const String& name) const										{ return getParamTexture(name).get(); }
		HSamplerState getSamplerState(const String& name) const								{ return getParamSamplerState(name).get(); }
		float getFloat(const String& name, UINT32 arrayIdx = 0) const						{ return getParamFloat(name).get(arrayIdx); }
		Vector2 getVec2(const String& name, UINT32 arrayIdx = 0) const						{ return getParamVec2(name).get(arrayIdx); }
		Vector3 getVec3(const String& name, UINT32 arrayIdx = 0) const						{ return getParamVec3(name).get(arrayIdx); }
		Vector4 getVec4(const String& name, UINT32 arrayIdx = 0) const						{ return getParamVec4(name).get(arrayIdx); }
		Matrix3 getMat3(const String& name, UINT32 arrayIdx = 0) const						{ return getParamMat3(name).get(arrayIdx); }
		Matrix4 getMat4(const String& name, UINT32 arrayIdx = 0) const						{ return getParamMat4(name).get(arrayIdx); }
		StructData getStructData(const String& name, UINT32 arrayIdx = 0) const;

		GpuParamFloat getParamFloat(const String& name) const;
		GpuParamVec2 getParamVec2(const String& name) const;
		GpuParamVec3 getParamVec3(const String& name) const;
		GpuParamVec4 getParamVec4(const String& name) const;
		GpuParamMat3 getParamMat3(const String& name) const;
		GpuParamMat4 getParamMat4(const String& name) const;
		GpuParamStruct getParamStruct(const String& name) const;
		GpuParamTexture getParamTexture(const String& name) const;
		GpuParamSampState getParamSamplerState(const String& name) const;

		INT16 getRenderQueue() const { return mRenderQueue; }

		UINT32 getNumPasses() const;

		PassPtr getPass(UINT32 passIdx) const;

		PassParametersPtr getPassParameters(UINT32 passIdx) const;

		static HMaterial create();
		static HMaterial create(ShaderPtr shader);
	protected:
		void destroy_internal();

		/**
		 * @brief	Allows you to retrieve a handle to a parameter that you can then use for quickly
		 * 			setting and retrieving parameter data. This allows you to set/get parameter data
		 * 			without all the cost of extra lookups otherwise required.
		 * 			
		 * @note	All of these handles will be invalidated if material shader ever changes. It is up to the
		 * 			caller to keep track of that.
		 */
		template <typename T>
		void getParam(const String& name, GpuDataParamBase<T>& output) const
		{
			throwIfNotInitialized();

			auto iterFind = mValidParams.find(name);
			if(iterFind == mValidParams.end())
			{
				LOGWRN("Material doesn't have a parameter named " + name);
				return;
			}

			const String& gpuVarName = iterFind->second;
			GpuParamsPtr params = findParamsWithName(gpuVarName);

			params->getParam<T>(gpuVarName, output);
		}

	private:
		friend class MaterialManager;

		ShaderPtr mShader;
		TechniquePtr mBestTechnique;
		INT16 mRenderQueue;

		Set<String>::type mValidShareableParamBlocks;
		Map<String, String>::type mValidParams; // Also maps Shader param name -> gpu variable name

		Vector<PassParametersPtr>::type mParametersPerPass;
		Vector<GpuParamBlockBufferPtr>::type mParamBuffers;

		Material();

		void throwIfNotInitialized() const;

		const Map<String, String>::type& getValidParamNames() const { return mValidParams; }

		void initBestTechnique();

		Map<String, const GpuParamDataDesc*>::type determineValidDataParameters(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Set<String>::type determineValidObjectParameters(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Set<String>::type determineValidShareableParamBlocks(const Vector<const GpuParamDesc*>::type& paramDescs) const;
		Map<String, String>::type determineParameterToBlockMapping(const Vector<const GpuParamDesc*>::type& paramDescs);

		bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets = false) const;

		void freeParamBuffers();

		GpuParamsPtr findParamsWithName(const String& name) const;
		GpuParamsPtr findTexWithName(const String& name) const;
		GpuParamsPtr findSamplerStateWithName(const String& name) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}