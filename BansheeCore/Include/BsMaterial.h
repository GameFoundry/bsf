//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"
#include "BsGpuParam.h"
#include "BsMaterialProxy.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsMatrix3.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of material dirty flags
	 */
	enum class MaterialDirtyFlag
	{
		Material = 0x01, /**< Internal material data is dirty. */
		Proxy = 0x02, /**< Active proxy needs to be updated. */
		Params = 0x04 /**< Parameters are dirty. */
	};

	/**
	 * @brief	Helper class containing parameters for all types
	 * 			of GPU programs used in a pass.
	 */
	class BS_CORE_EXPORT PassParameters
	{
	public:
		GpuParamsPtr mVertParams;
		GpuParamsPtr mFragParams;
		GpuParamsPtr mGeomParams;
		GpuParamsPtr mHullParams;
		GpuParamsPtr mDomainParams;
		GpuParamsPtr mComputeParams;

		/**
		 * @brief	Returns a set of GPU parameters based on an index.
		 *
		 * @note	Useful when needing to iterate over all sets of GPU parameters.
		 */
		GpuParamsPtr& getParamByIdx(UINT32 idx)
		{
			GpuParamsPtr* paramArray[] = {&mVertParams, &mFragParams, &mGeomParams, &mHullParams, &mDomainParams, &mComputeParams};

			return *paramArray[idx];
		}

		/**
		 * @brief	Sets GPU parameters based on an index.
		 *
		 * @note	Useful when needing to iterate over all sets of GPU parameters.
		 */
		void setParamByIdx(UINT32 idx, const GpuParamsPtr& params)
		{
			GpuParamsPtr* paramArray[] = {&mVertParams, &mFragParams, &mGeomParams, &mHullParams, &mDomainParams, &mComputeParams};

			(*paramArray[idx]) = params;
		}

		/**
		 * @brief	Returns the total number of stored sets of 
		 * 			GPU parameters in this object.
		 */
		UINT32 getNumParams() const { return 6; }
	};

	/**
	 * @brief	Material represents a shader and parameters used to set up
	 * 			that shader. It provides a simple interface for manipulating the
	 * 			parameters.
	 */
	class BS_CORE_EXPORT Material : public Resource
	{
	public:
		/**
		 * @brief	Data used to described a structure defined within a shader.
		 */
		struct StructData
		{
			StructData()
				:size(0), data(nullptr)
			{ }

			StructData(UINT32 _size)
				:size(_size)
			{
				data = std::shared_ptr<void>(bs_alloc<ScratchAlloc>(_size), &bs_free<ScratchAlloc>);
			}

			/**
			 * @brief	Writes the specified data to the internal buffer. Caller
			 * 			must ensure size of the provided buffer is correct.
			 */
			void write(void* _data)
			{
				memcpy(data.get(), _data, size);
			}

			std::shared_ptr<void> data;
			UINT32 size;
		};

		~Material();

		/**
		 * @brief	Sets a shader that will be used by the material. Best technique within the
		 * 			provided shader will be used for the material.
		 *
		 * @note	Shader must be set before doing any other operations with the material.
		 * 			
		 * 			After setting the shader if change any systems that a shader technique is 
		 * 			dependent upon (render system, renderer, etc), you will need to call this 
		 * 			method again on all your Materials to make sure technique used is updated.
		 */
		void setShader(ShaderPtr shader);

		/**
		 * @brief	Returns the currently active shader.
		 */
		ShaderPtr getShader() const { return mShader; }

		/** @brief	Assigns a texture to the shader parameter with the specified name. */
		void setTexture(const String& name, const HTexture& value) { return getParamTexture(name).set(value); }

		/** @brief	Assigns a sampler state to the shader parameter with the specified name. */
		void setSamplerState(const String& name, const HSamplerState& value) { return getParamSamplerState(name).set(value); }

		/**   
		 *  @brief	Assigns a float value to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setFloat(const String& name, float value, UINT32 arrayIdx = 0)	{ return getParamFloat(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a color to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setColor(const String& name, const Color& value, UINT32 arrayIdx = 0);

		/**   
		 *  @brief	Assigns a 2D vector to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setVec2(const String& name, const Vector2& value, UINT32 arrayIdx = 0)	{ return getParamVec2(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a 3D vector to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setVec3(const String& name, const Vector3& value, UINT32 arrayIdx = 0)	{ return getParamVec3(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a 4D vector to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setVec4(const String& name, const Vector4& value, UINT32 arrayIdx = 0)	{ return getParamVec4(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a 3x3 matrix to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setMat3(const String& name, const Matrix3& value, UINT32 arrayIdx = 0)	{ return getParamMat3(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a 4x4 matrix to the shader parameter with the specified name. 
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setMat4(const String& name, const Matrix4& value, UINT32 arrayIdx = 0)	{ return getParamMat4(name).set(value, arrayIdx); }

		/**   
		 *  @brief	Assigns a structure to the shader parameter with the specified name.
		 *  		
		 *			Structure is provided as a raw buffer and caller must ensure structure in buffer
		 *			matches what the shader expects.
		 *
		 *			Optionally if the parameter is an array you may provide an array index to assign the value to.
		 */
		void setStructData(const String& name, void* value, UINT32 size, UINT32 arrayIdx = 0) { return getParamStruct(name).set(value, size, arrayIdx); }

		/**
		 * @brief	Assign a parameter block buffer with the specified name.
		 *
		 * @note	Parameter block buffers can be used as quick way of setting multiple parameters on a material at once, or
		 * 			potentially sharing parameters between multiple materials. This reduces driver overhead as the parameters
		 * 			in the buffers need only be set once and then reused multiple times.
		 */
		void setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlock);

		/** @brief	Returns a texture assigned with the parameter with the specified name. */
		HTexture getTexture(const String& name) const { return getParamTexture(name).get(); }

		/** @brief	Returns a sampler state assigned with the parameter with the specified name. */
		HSamplerState getSamplerState(const String& name) const	{ return getParamSamplerState(name).get(); }

		/**
		 * @brief	Returns a float value assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		float getFloat(const String& name, UINT32 arrayIdx = 0) const { return getParamFloat(name).get(arrayIdx); }

		/**
		 * @brief	Returns a 2D vector assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Vector2 getVec2(const String& name, UINT32 arrayIdx = 0) const { return getParamVec2(name).get(arrayIdx); }

		/**
		 * @brief	Returns a 3D vector assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Vector3 getVec3(const String& name, UINT32 arrayIdx = 0) const { return getParamVec3(name).get(arrayIdx); }

		/**
		 * @brief	Returns a 4D vector assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Vector4 getVec4(const String& name, UINT32 arrayIdx = 0) const { return getParamVec4(name).get(arrayIdx); }

		/**
		 * @brief	Returns a 3x3 matrix assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Matrix3 getMat3(const String& name, UINT32 arrayIdx = 0) const { return getParamMat3(name).get(arrayIdx); }

		/**
		 * @brief	Returns a 4x4 matrix assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Matrix4 getMat4(const String& name, UINT32 arrayIdx = 0) const { return getParamMat4(name).get(arrayIdx); }

		/**
		 * @brief	Returns a buffer representing a structure assigned to the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		StructData getStructData(const String& name, UINT32 arrayIdx = 0) const;

		/**
		 * @brief	Returns a float GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamFloat getParamFloat(const String& name) const;

		/**
		 * @brief	Returns a 2D vector GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamVec2 getParamVec2(const String& name) const;

		/**
		 * @brief	Returns a 3D vector GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamVec3 getParamVec3(const String& name) const;

		/**
		 * @brief	Returns a 4D vector GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamVec4 getParamVec4(const String& name) const;

		/**
		 * @brief	Returns a 3x3 matrix GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamMat3 getParamMat3(const String& name) const;

		/**
		 * @brief	Returns a 4x4 matrix GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamMat4 getParamMat4(const String& name) const;

		/**
		 * @brief	Returns a structure GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamStruct getParamStruct(const String& name) const;

		/**
		 * @brief	Returns a texture GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamTexture getParamTexture(const String& name) const;

		/**
		 * @brief	Returns a sampler state GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		GpuParamSampState getParamSamplerState(const String& name) const;

		/**
		 * @brief	Returns the number of passes that are used
		 * 			by the shader contained in the material.
		 */
		UINT32 getNumPasses() const;

		/**
		 * @brief	Retrieves a specific shader pass.
		 */
		PassPtr getPass(UINT32 passIdx) const;

		/**
		 * @brief	Returns a set of parameters for all GPU programs
		 * 			in the specified shader pass.
		 */
		PassParametersPtr getPassParameters(UINT32 passIdx) const;

		/**
		 * @brief	Creates a new empty material.
		 * 			
		 * @note	Make sure you call Material::setShader before using it.
		 */
		static HMaterial create();

		/**
		 * @brief	Creates a new material with the specified shader.
		 */
		static HMaterial create(ShaderPtr shader);

		/************************************************************************/
		/* 								CORE PROXY                      		*/
		/************************************************************************/

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and core thread potentially needs to be notified.
		 *
		 * @note	Sim thread only.
		 */
		bool _isCoreDirty(MaterialDirtyFlag flag) const;

		/**
		 * @brief	Marks the core dirty flag as clean.
		 *
		 * @note	Sim thread only.
		 */
		void _markCoreClean(MaterialDirtyFlag flag);

		/**
		 * @brief	Gets the currently active proxy of this material.
		 */
		MaterialProxyPtr _getActiveProxy() const { return mActiveProxy; }

		/**
		 * @brief	Sets an active proxy for this material.
		 */
		void _setActiveProxy(const MaterialProxyPtr& proxy) { mActiveProxy = proxy; }

		/**
		 * @brief	Returns updated GPU parameters since the last time the parameters were marked clean.
		 */
		Vector<MaterialProxy::ParamsBindInfo> _getDirtyProxyParams();

		/**
		 * @brief	Creates a new core proxy from the currently set material data. Core proxies ensure
		 *			that the core thread has all the necessary material data, while avoiding the need
		 *			to manage Material itself on the core thread.
		 *
		 * @note	Sim thread only. 
		 *			You generally need to update the core thread with a new proxy whenever core 
		 *			dirty flag is set.
		 */
		MaterialProxyPtr _createProxy();
	protected:
		/**
		 * @copydoc	Resource::destroy_internal
		 */
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
		void getParam(const String& name, TGpuDataParam<T>& output) const
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
		INT32 mCoreDirtyFlags;

		Set<String> mValidShareableParamBlocks;
		Map<String, String> mValidParams; // Also maps Shader param name -> gpu variable name

		Vector<PassParametersPtr> mParametersPerPass;
		Vector<GpuParamBlockBufferPtr> mParamBuffers;

		MaterialProxyPtr mActiveProxy;

		Material();

		/**
		 * @brief	Throw an exception if no shader is set, or no acceptable
		 * 			technique was found.
		 */
		void throwIfNotInitialized() const;

		/**
		 * @brief	Marks the core data as dirty.
		 */
		void markCoreDirty() { mCoreDirtyFlags = 0xFFFFFFFF; }

		/**
		 * @brief	Retrieves a list of all shader GPU parameters, and the GPU program variable names they map to.
		 */
		const Map<String, String>& getValidParamNames() const { return mValidParams; }

		/**
		 * @brief	Initializes the material by using the best technique from the currently set shader. Shader
		 * 			must contain the technique that matches the current renderer and render system.
		 */
		void initBestTechnique();

		/**
		 * @brief	Constructs a map containing all data parameters (e.g. float, vector3, color).
		 * 			Map contains parameter names and descriptions.
		 */
		Map<String, const GpuParamDataDesc*> determineValidDataParameters(const Vector<GpuParamDescPtr>& paramDescs) const;

		/**
		 * @brief	Constructs a list containing all object parameter (e.g. texture, sampler state) names.
		 */
		Set<String> determineValidObjectParameters(const Vector<GpuParamDescPtr>& paramDescs) const;

		/**
		 * @brief	Constructs a list containing all shareable parameter block names. Shareable blocks may be shared between
		 * 			different GPU programs, passes or even materials. 
		 */
		Set<String> determineValidShareableParamBlocks(const Vector<GpuParamDescPtr>& paramDescs) const;

		/**
		 * @brief	Constructs a map that maps parameter names to a parameter block.
		 */
		Map<String, String> determineParameterToBlockMapping(const Vector<GpuParamDescPtr>& paramDescs);

		/**
		 * @brief	Checks are the specified two parameters equal
		 *
		 * @param	paramA			   	The parameter a to compare.
		 * @param	paramB			   	The parameter b to compare.
		 * @param	ignoreBufferOffsets	(optional) If true, parameter offsets into the parameter buffer will be ignored
		 * 								when comparing.
		 */
		bool areParamsEqual(const GpuParamDataDesc& paramA, const GpuParamDataDesc& paramB, bool ignoreBufferOffsets = false) const;

		/**
		 * @brief	Frees all parameter block buffers.
		 */
		void freeParamBuffers();

		/**
		 * @brief	Finds a set of GPU parameters containing a data (e.g. float, vector2) parameter with the provided name.
		 */
		GpuParamsPtr findParamsWithName(const String& name) const;

		/**
		 * @brief	Finds a set of GPU parameters containing a texture parameter with the provided name.
		 */
		GpuParamsPtr findTexWithName(const String& name) const;

		/**
		 * @brief	Finds a set of GPU parameters containing a sampler state parameter with the provided name.
		 */
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