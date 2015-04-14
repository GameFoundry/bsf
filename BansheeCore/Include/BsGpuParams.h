#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParam.h"
#include "BsCoreObject.h"
#include "BsIResourceListener.h"

#include "BsDebug.h"

namespace BansheeEngine
{
	/**
	 * @brief	Stores information needed for binding a texture to the pipeline.
	 */
	struct BoundTextureInfo
	{
		BoundTextureInfo()
			:isLoadStore(false)
		{ }

		bool isLoadStore;
		TextureSurface surface;
	};

	/**
	 * @brief	Contains functionality common for both sim and core thread
	 *			version of GpuParams.
	 */
	class BS_CORE_EXPORT GpuParamsBase
	{
	public:
		/**
		 * @brief	Creates new GpuParams object using the specified parameter descriptions.
		 *
		 * @param	paramDesc			Reference to parameter descriptions that will be used for
		 *								finding needed parameters.
		 * @param	transposeMatrices	If true the stored matrices will be transposed before
		 *								submitted to the GPU (some APIs require different
		 *								matrix layout).
		 *
		 * @note	You normally do not want to call this manually. Instead use GpuProgram::createParameters.
		 */
		GpuParamsBase(const GpuParamDescPtr& paramDesc, bool transposeMatrices);
		virtual ~GpuParamsBase();

		// Note: Disallow copy/assign because it would require some care when copying (copy internal data shared_ptr and
		// all the internal buffers too). Trivial to implement but not needed at this time. Un-delete and implement if necessary.
		GpuParamsBase(const GpuParamsBase& other) = delete;
		GpuParamsBase& operator=(const GpuParamsBase& rhs) = delete;

		/**
		 * @brief	Returns a description of all stored parameters.
		 */
		const GpuParamDesc& getParamDesc() const { return *mParamDesc; }

		/**
		 * @brief	Returns the size of a data parameter with the specified name, in bytes.
		 *			Returns 0 if such parameter doesn't exist.
		 */
		UINT32 getDataParamSize(const String& name) const;

		/**
		 * @brief	Checks if parameter with the specified name exists.
		 */
		bool hasParam(const String& name) const;

		/**
		 * @brief	Checks if texture parameter with the specified name exists.
		 */
		bool hasTexture(const String& name) const;

		/**
		 * @brief	Checks if sampler state parameter with the specified name exists.
		 */
		bool hasSamplerState(const String& name) const;

		/**
		 * @brief	Checks if a parameter block with the specified name exists.
		 */
		bool hasParamBlock(const String& name) const;

		/**
		 * @brief	Checks is the texture at the specified slot to be bound as
		 *			random load/store texture instead of a normal sampled texture.
		 */
		bool isLoadStoreTexture(UINT32 slot) const;

		/**
		 * @brief	Changes the type of the texture at the specified slot.
		 */
		void setIsLoadStoreTexture(UINT32 slot, bool isLoadStore);

		/**
		 * @brief	Returns information that determines which texture surfaces to bind
		 *			as load/store parameters.
		 */
		const TextureSurface& getLoadStoreSurface(UINT32 slot) const;

		/**
		 * @brief	Sets information that determines which texture surfaces to bind
		 *			as load/store parameters.
		 */
		void setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const;

		/**
		 * @brief	Checks whether matrices should be transformed before
		 *			being written to the parameter buffer.
		 */
		bool getTransposeMatrices() const { return mTransposeMatrices; }

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 *
		 * @note	Internal method.
		 */
		virtual void _markCoreDirty() { }

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 *
		 * @note	Internal method.
		 */
		virtual void _markResourcesDirty() { }

	protected:
		/**
		 * @brief	Gets a descriptor for a data parameter with the specified name.
		 */
		GpuParamDataDesc* getParamDesc(const String& name) const;

		GpuParamDescPtr mParamDesc;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		BoundTextureInfo* mTextureInfo;

		bool mTransposeMatrices;
	};

	/**
	 * @brief	Templated version of GpuParams that contains functionality for both
	 *			sim and core thread versions of stored data.
	 */
	template <bool Core>
	class BS_CORE_EXPORT TGpuParams : public GpuParamsBase
	{
	public:
		template<bool Core> struct TTypes { };

		template<> struct TTypes < false > 
		{ 
			typedef GpuParams GpuParamsType; 
			typedef HTexture TextureType;
			typedef SamplerStatePtr SamplerType;
			typedef SPtr<GpuParamBlockBuffer> ParamsBufferType;
		};

		template<> struct TTypes < true > 
		{ 
			typedef GpuParamsCore GpuParamsType;
			typedef SPtr<TextureCore> TextureType;
			typedef SPtr<SamplerStateCore> SamplerType;
			typedef SPtr<GpuParamBlockBufferCore> ParamsBufferType;
		};

		typedef typename TTypes<Core>::GpuParamsType GpuParamsType;
		typedef typename TTypes<Core>::TextureType TextureType;
		typedef typename TTypes<Core>::SamplerType SamplerType;
		typedef typename TTypes<Core>::ParamsBufferType ParamsBufferType;

		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase(const GpuParamDescPtr&, bool)
		 */
		TGpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

		virtual ~TGpuParams();

		/**
		 * @brief	Binds a new parameter buffer to the specified slot. Any following parameter reads or
		 *			writes that are referencing that buffer slot will use the new buffer.
		 *
		 * @note	This is useful if you want to share a parameter buffer among multiple GPU programs. 
		 *			You would only set the values once and then share the buffer among all other GpuParams.
		 *
		 *			It is up to the caller to guarantee the provided buffer matches parameter block
		 *			descriptor for this slot.
		 */
		void setParamBlockBuffer(UINT32 slot, const ParamsBufferType& paramBlockBuffer);

		/**
		 * @brief	Replaces the parameter buffer with the specified name. Any following parameter reads or
		 *			writes that are referencing that buffer will use the new buffer.
		 *
		 * @note	This is useful if you want to share a parameter buffer among multiple GPU programs.
		 *			You would only set the values once and then share the buffer among all other GpuParams.
		 *
		 *			It is up to the caller to guarantee the provided buffer matches parameter block
		 *			descriptor for this slot.
		 */
		void setParamBlockBuffer(const String& name, const ParamsBufferType& paramBlockBuffer);

		/**
		 * @brief	Returns a handle for the parameter with the specified name.
		 *			Handle may then be stored and used for quickly setting or retrieving
		 *			values to/from that parameter.
		 *
		 *			Throws exception if parameter with that name and type doesn't exist.
		*
		*			Parameter handles will be invalidated when their parent GpuParams object changes.
		*/
		template<class T> void getParam(const String& name, TGpuDataParam<T, Core>& output) const;

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getStructParam(const String& name, TGpuParamStruct<Core>& output) const;

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getTextureParam(const String& name, TGpuParamTexture<Core>& output) const;

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getLoadStoreTextureParam(const String& name, TGpuParamLoadStoreTexture<Core>& output) const;

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getSamplerStateParam(const String& name, TGpuParamSampState<Core>& output) const;

		/**
		 * @brief	Gets a parameter block buffer from the specified slot.
		 */
		ParamsBufferType getParamBlockBuffer(UINT32 slot) const;

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		TextureType getTexture(UINT32 slot);

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		SamplerType getSamplerState(UINT32 slot);

		/**
		 * @brief	Sets a texture at the specified slot.
		 */
		void setTexture(UINT32 slot, const TextureType& texture);

		/**
		 * @brief	Sets a sampler state at the specified slot.
		 */
		void setSamplerState(UINT32 slot, const SamplerType& sampler);

	protected:
		/**
		 * @copydoc	CoreObject::getThisPtr
		 */
		virtual SPtr<GpuParamsType> _getThisPtr() const = 0;

		ParamsBufferType* mParamBlockBuffers;
		TextureType* mTextures;
		SamplerType* mSamplerStates;
	};

	/**
	 * @brief	Core thread version of GpuParams.
	 *
	 * @see		GpuParams
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuParamsCore : public CoreObjectCore, public TGpuParams<true>
	{
	public:
		~GpuParamsCore() { }

		/**
		 * @brief	Uploads all CPU stored parameter buffer data to the GPU buffers.
		 */
		void updateHardwareBuffers();

		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase
		 */
		static SPtr<GpuParamsCore> create(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

	protected:
		friend class GpuParams;

		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase
		 */
		GpuParamsCore(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

		/**
		 * @copydoc	CoreObject::getThisPtr
		 */
		SPtr<GpuParamsCore> _getThisPtr() const;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		void syncToCore(const CoreSyncData& data);
	};

	/**
	 * @brief	Contains descriptions for all parameters in a GPU program and also
	 *			allows you to write and read those parameters. All parameter values
	 *			are stored internally on the CPU, and are only submitted to the GPU
	 *			once the parameters are bound to the pipeline.
	 *
	 * @see		CoreThreadAccessor::bindGpuParams
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GpuParams : public CoreObject, public TGpuParams<false>, public IResourceListener
	{
	public:
		~GpuParams() { }

		/**
		 * @copydoc	CoreObject::markCoreDirty
		 *
		 * @note	Internal method.
		 */
		void _markCoreDirty();

		/**
		 * @copydoc	IResourceListener::markResourcesDirty
		 *
		 * @note	Internal method.
		 */
		void _markResourcesDirty();

		/**
		 * @brief	Retrieves a core implementation of a mesh usable only from the
		 *			core thread.
		 */
		SPtr<GpuParamsCore> getCore() const;

		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase
		 */
		static SPtr<GpuParams> create(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

	protected:
		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase
		 */
		GpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

		/**
		 * @copydoc	CoreObject::getThisPtr
		 */
		SPtr<GpuParams> _getThisPtr() const;

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObject::syncToCore
		 */
		CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @copydoc	IResourceListener::getResourceDependencies
		 */
		void getListenerResources(Vector<HResource>& resources);

		/**
		 * @copydoc IResourceListener::notifyResourceLoaded
		 */
		void notifyResourceLoaded(const HResource& resource) { markCoreDirty(); }

		/**
		 * @copydoc IResourceListener::notifyResourceDestroyed
		 */
		void notifyResourceDestroyed(const HResource& resource) { markCoreDirty(); }

		/**
		 * @copydoc IResourceListener::notifyResourceChanged
		 */
		void notifyResourceChanged(const HResource& resource) { markCoreDirty(); }
	};
}