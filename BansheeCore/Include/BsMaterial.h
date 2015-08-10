#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"
#include "BsIResourceListener.h"
#include "BsMaterialParam.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsMatrix3.h"
#include "BsMatrix4.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class containing parameters for all types
	 * 			of GPU programs used in a pass.
	 */
	template<bool Core>
	class TPassParameters
	{
	public:
		typedef typename TGpuParamsPtrType<Core>::Type GpuParamsType;

		/**
		 * @brief	Returns a set of GPU parameters based on an index.
		 *
		 * @note	Useful when needing to iterate over all sets of GPU parameters.
		 */
		GpuParamsType& getParamByIdx(UINT32 idx)
		{
			GpuParamsType* paramArray[] = { &mVertParams, &mFragParams, &mGeomParams, &mHullParams, &mDomainParams, &mComputeParams };

			return *paramArray[idx];
		}

		/**
		 * @brief	Sets GPU parameters based on an index.
		 *
		 * @note	Useful when needing to iterate over all sets of GPU parameters.
		 */
		void setParamByIdx(UINT32 idx, const GpuParamsType& params)
		{
			GpuParamsType* paramArray[] = { &mVertParams, &mFragParams, &mGeomParams, &mHullParams, &mDomainParams, &mComputeParams };

			(*paramArray[idx]) = params;
		}

		GpuParamsType mVertParams;
		GpuParamsType mFragParams;
		GpuParamsType mGeomParams;
		GpuParamsType mHullParams;
		GpuParamsType mDomainParams;
		GpuParamsType mComputeParams;

		static const UINT32 NUM_PARAMS;
	};

	template<bool Core> const UINT32 TPassParameters<Core>::NUM_PARAMS = 6;

	template<bool Core> struct TGpuParamBlockBufferPtrType { };
	template<> struct TGpuParamBlockBufferPtrType<false> { typedef SPtr<GpuParamBlockBuffer> Type; };
	template<> struct TGpuParamBlockBufferPtrType<true> { typedef SPtr<GpuParamBlockBufferCore> Type; };

	template<bool Core> struct TGpuProgramType { };
	template<> struct TGpuProgramType<false> { typedef GpuProgramPtr Type; };
	template<> struct TGpuProgramType<true> { typedef SPtr<GpuProgramCore> Type; };

	typedef TPassParameters<false> PassParameters;
	typedef TPassParameters<true> PassParametersCore;

	/**
	 * @brief	Material represents a shader and parameters used to set up
	 * 			that shader. It provides a simple interface for manipulating the
	 * 			parameters.
	 */
	class BS_CORE_EXPORT MaterialBase
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

		virtual ~MaterialBase() { }

	protected:
		/**
		 * @brief	Retrieves a list of all shader GPU parameters, and the GPU program variable names they map to.
		 */
		const Map<String, String>& getValidParamNames() const { return mValidParams; }

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		virtual void _markCoreDirty() { }

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 */
		virtual void _markResourcesDirty() { }

		/**
		 * @brief	Returns all GPU parameter descriptions in the specified technique.
		 */
		static Vector<GpuParamDescPtr> getAllParamDescs(const SPtr<Technique>& technique);

		/**
		 * @brief	Returns all GPU parameter descriptions in the specified technique.
		 */
		static Vector<GpuParamDescPtr> getAllParamDescs(const SPtr<TechniqueCore>& technique);

		Set<String> mValidShareableParamBlocks;
		Map<String, String> mValidParams; // Also maps Shader param name -> gpu variable name
	};

	/**
	 * @copydoc	MaterialBase
	 */
	template<bool Core>
	class BS_CORE_EXPORT TMaterial : public MaterialBase
	{
	public:
		template<bool Core> struct TPassType {};
		template<> struct TPassType < false > { typedef SPtr<Pass> Type; };
		template<> struct TPassType < true > { typedef SPtr<PassCore> Type; };

		template<bool Core> struct TTechniqueType {};
		template<> struct TTechniqueType < false > { typedef SPtr<Technique> Type; };
		template<> struct TTechniqueType < true > { typedef SPtr<TechniqueCore> Type; };

		template<bool Core> struct TShaderType {};
		template<> struct TShaderType < false > { typedef HShader Type; };
		template<> struct TShaderType < true > { typedef SPtr<ShaderCore> Type; };

		template<bool Core> struct TGpuParamBlockBufferType {};
		template<> struct TGpuParamBlockBufferType < false > { typedef GpuParamBlockBuffer Type; };
		template<> struct TGpuParamBlockBufferType < true > { typedef GpuParamBlockBufferCore Type; };

		typedef typename TGpuParamsPtrType<Core>::Type GpuParamsType;
		typedef typename TGpuParamTextureType<Core>::Type TextureType;
		typedef typename TGpuParamSamplerStateType<Core>::Type SamplerStateType;
		typedef typename TGpuParamBlockBufferPtrType<Core>::Type ParamBlockPtrType;
		typedef typename TGpuParamBlockBufferType<Core>::Type ParamBlockType;
		typedef typename TGpuProgramType<Core>::Type GpuProgramType;
		typedef typename TPassType<Core>::Type PassType;
		typedef typename TTechniqueType<Core>::Type TechniqueType;
		typedef typename TShaderType<Core>::Type ShaderType;

		virtual ~TMaterial() { }

		/**
		 * @brief	Returns the currently active shader.
		 */
		ShaderType getShader() const { return mShader; }

		/**
		 * @brief	Returns the number of passes that are used
		 * 			by the shader used in the material.
		 */
		UINT32 getNumPasses() const;

		/**
		 * @brief	Retrieves a specific shader pass.
		 */
		PassType getPass(UINT32 passIdx) const;

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
		void setColor(const String& name, const Color& value, UINT32 arrayIdx = 0)
		{
			return getParamVec4(name).set(Vector4(value.r, value.g, value.b, value.a), arrayIdx);
		}

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

		/** @brief	Assigns a texture to the shader parameter with the specified name. */
		void setTexture(const String& name, const TextureType& value) { return getParamTexture(name).set(value); }

		/** 
		 * @brief	Assigns a texture to be used for random load/store operations to the
		 *			shader parameter with the specified name.
		 */
		void setLoadStoreTexture(const String& name, const TextureType& value, const TextureSurface& surface)
		{ 
			return getParamLoadStoreTexture(name).set(value, surface); 
		}

		/** @brief	Assigns a sampler state to the shader parameter with the specified name. */
		void setSamplerState(const String& name, const SamplerStateType& value) { return getParamSamplerState(name).set(value); }

		/**
		 * @brief	Returns a float value assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		float getFloat(const String& name, UINT32 arrayIdx = 0) const { return getParamFloat(name).get(arrayIdx); }

		/**
		 * @brief	Returns a color assigned with the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		Color getColor(const String& name, UINT32 arrayIdx = 0) const { return getParamColor(name).get(arrayIdx); }

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

		/** @brief	Returns a texture assigned with the parameter with the specified name. */
		TextureType getTexture(const String& name) const { return getParamTexture(name).get(); }

		/** @brief	Returns a sampler state assigned with the parameter with the specified name. */
		SamplerStateType getSamplerState(const String& name) const	{ return getParamSamplerState(name).get(); }

		/**
		 * @brief	Returns a buffer representing a structure assigned to the parameter with the specified name.
		 *
		 *			Optionally if the parameter is an array you may provide an array index you which to retrieve.
		 */
		MaterialBase::StructData getStructData(const String& name, UINT32 arrayIdx = 0) const
		{
			TMaterialParamStruct<Core> structParam = getParamStruct(name);

			MaterialBase::StructData data(structParam.getElementSize());
			structParam.get(data.data.get(), structParam.getElementSize(), arrayIdx);

			return data;
		}

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
		TMaterialDataParam<float, Core> getParamFloat(const String& name) const
		{
			TMaterialDataParam<float, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

		/**
		 * @brief	Returns a color GPU parameter. This parameter may be used for
		 * 			more efficiently getting/setting GPU parameter values than calling
		 * 			Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		TMaterialDataParam<Color, Core> getParamColor(const String& name) const
		{
			TMaterialDataParam<Color, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialDataParam<Vector2, Core> getParamVec2(const String& name) const
		{
			TMaterialDataParam<Vector2, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialDataParam<Vector3, Core> getParamVec3(const String& name) const
		{
			TMaterialDataParam<Vector3, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialDataParam<Vector4, Core> getParamVec4(const String& name) const
		{
			TMaterialDataParam<Vector4, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialDataParam<Matrix3, Core> getParamMat3(const String& name) const
		{
			TMaterialDataParam<Matrix3, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialDataParam<Matrix4, Core> getParamMat4(const String& name) const
		{
			TMaterialDataParam<Matrix4, Core> gpuParam;
			getParam(name, gpuParam);

			return gpuParam;
		}

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
		TMaterialParamStruct<Core> getParamStruct(const String& name) const;

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
		TMaterialParamTexture<Core> getParamTexture(const String& name) const;

		/**
		 * @brief	Returns a GPU parameter for binding a load/store texture. This parameter 
		 *			may be used for more efficiently getting/setting GPU parameter values 
		 *			than calling Material::get* / Material::set* methods. 
		 *
		 * @note	Expected behavior is that you would retrieve this parameter when
		 * 			initially constructing the material, and then use it throughout material
		 * 			lifetime to assign and retrieve parameter values.
		 * 			
		 *			If material shader changes this handle will be invalidated.
		 */
		TMaterialParamLoadStoreTexture<Core> getParamLoadStoreTexture(const String& name) const;

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
		TMaterialParamSampState<Core> getParamSamplerState(const String& name) const;

		/**
		 * @brief	Returns a set of parameters for all GPU programs
		 * 			in the specified shader pass.
		 */
		SPtr<TPassParameters<Core>> getPassParameters(UINT32 passIdx) const { return mParametersPerPass[passIdx]; }

		/**
		 * @brief	Assign a parameter block buffer with the specified name.
		 *
		 * @note	Parameter block buffers can be used as quick way of setting multiple parameters on a material at once, or
		 * 			potentially sharing parameters between multiple materials. This reduces driver overhead as the parameters
		 * 			in the buffers need only be set once and then reused multiple times.
		 */
		void setParamBlockBuffer(const String& name, const ParamBlockPtrType& paramBlock);

	protected:
		/**
		 * @brief	Allows you to retrieve a handle to a parameter that you can then use for quickly
		 * 			setting and retrieving parameter data. This allows you to set/get parameter data
		 * 			without all the cost of extra lookups otherwise required.
		 * 			
		 * @note	All of these handles will be invalidated if material shader ever changes. It is up to the
		 * 			caller to keep track of that.
		 */
		template <typename T>
		void getParam(const String& name, TMaterialDataParam<T, Core>& output) const;

		/**
		 * @brief	Assigns a value from a raw buffer to the parameter with the specified name.
		 *			Buffer must be of sizeof(T) * numElements size and initialized.
		 *
		 * @note	Provided parameter must exist, no checking is done.
		 */
		template <typename T>
		void setParamValue(const String& name, UINT8* buffer, UINT32 numElements);

		/**
		 * @brief	Initializes the material by using the best technique from the currently set shader. Shader
		 * 			must contain the technique that matches the current renderer and render system.
		 */
		void initBestTechnique();

		/**
		 * @brief	Assigns all the default parameters specified in the shader to the material.
		 */
		void initDefaultParameters();

		/**
		 * @brief	Throw an exception if no shader is set, or no acceptable
		 * 			technique was found.
		 */
		void throwIfNotInitialized() const;

		Vector<SPtr<TPassParameters<Core>>> mParametersPerPass;
		ShaderType mShader;
		TechniqueType mBestTechnique;
	};

	/**
	 * @copydoc	MaterialBase
	 */
	class BS_CORE_EXPORT MaterialCore : public CoreObjectCore, public TMaterial<true>
	{
	public:
		~MaterialCore() { }

		/**
		 * @copydoc	Material::setShader
		 */
		void setShader(const SPtr<ShaderCore>& shader);

		/**
		 * @brief	Creates a new material with the specified shader.
		 */
		static SPtr<MaterialCore> create(const SPtr<ShaderCore>& shader);

	private:
		friend class Material;

		MaterialCore() { }
		MaterialCore(const SPtr<ShaderCore>& shader);
		MaterialCore(const SPtr<ShaderCore>& shader, const SPtr<TechniqueCore>& bestTechnique, 
			const Set<String>& validShareableParamBlocks, const Map<String, String>& validParams, 
			const Vector<SPtr<PassParametersCore>>& passParams);

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		void syncToCore(const CoreSyncData& data) override;
	};

	/**
	 * @copydoc	MaterialBase
	 */
	class BS_CORE_EXPORT Material : public Resource, public TMaterial<false>, public IResourceListener
	{
	public:
		~Material() { }

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
		void setShader(const HShader& shader);

		/**
		 * @brief	Retrieves an implementation of a material usable only from the
		 *			core thread.
		 */
		SPtr<MaterialCore> getCore() const;

		/**
		 * @copydoc	CoreObject::initialize
		 */
		void initialize() override;

		/**
		 * @brief	Creates a new empty material.
		 * 			
		 * @note	Make sure you call Material::setShader before using it.
		 */
		static HMaterial create();

		/**
		 * @brief	Creates a new material with the specified shader.
		 */
		static HMaterial create(const HShader& shader);

	private:
		friend class MaterialManager;

		Material();
		Material(const HShader& shader);

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const override;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies) override;

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 */
		void _markCoreDirty() override;

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 */
		void _markResourcesDirty() override;

		/**
		 * @copydoc	IResourceListener::getListenerResources
		 */
		void getListenerResources(Vector<HResource>& resources) override;

		/**
		 * @copydoc IResourceListener::notifyResourceLoaded
		 */
		void notifyResourceLoaded(const HResource& resource) override;

		/**
		 * @copydoc IResourceListener::notifyResourceDestroyed
		 */
		void notifyResourceDestroyed(const HResource& resource) override;

		/**
		 * @copydoc IResourceListener::notifyResourceChanged
		 */
		void notifyResourceChanged(const HResource& resource) override;

		/**
		 * @copydoc	Resource::getResourceDependencies
		 */
		void getResourceDependencies(FrameVector<HResource>& dependencies) const override;

		/**
		 * @brief	Performs material initialization when all resources are ready.
		 */
		void initializeIfLoaded();

		UINT32 mLoadFlags;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class MaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}