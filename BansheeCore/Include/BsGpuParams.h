#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParam.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	struct GpuParamsInternalData;

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

	template<bool Core>
	struct TGpuParamsBaseType
	{ };

	template<>
	struct TGpuParamsBaseType<true>
	{ 
		typedef CoreObjectCore Type;
	};

	template<>
	struct TGpuParamsBaseType<false>
	{
		typedef CoreObject Type;
	};

	template<bool Core>
	struct TGpuParamsType
	{ };

	template<>
	struct TGpuParamsType<false>
	{
		typedef GpuParams Type;
	};

	template<>
	struct TGpuParamsType<true>
	{
		typedef GpuParamsCore Type;
	};

	/**
	 * @brief	Templated version of GpuParams that contains functionality for both
	 *			sim and core thread versions of stored data.
	 */
	template <class TParamsBuffer, class TTexture, class TSampler, bool Core>
	class TGpuParams : public TGpuParamsBaseType<Core>::Type, public GpuParamsBase
	{
	public:
		/**
		 * @copydoc	GpuParamsBase::GpuParamsBase(const GpuParamDescPtr&, bool)
		 */
		TGpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
			:GpuParamsBase(paramDesc, transposeMatrices), mParamBlockBuffers(nullptr), mTextures(nullptr), 
			mSamplerStates(nullptr)
		{
			mParamBlockBuffers = bs_newN<TParamsBuffer>(mNumParamBlocks);
			mTextures = bs_newN<TTexture>(mNumTextures);
			mSamplerStates = bs_newN<TSampler>(mNumSamplerStates);
		}

		virtual ~TGpuParams()
		{
			bs_deleteN(mParamBlockBuffers, mNumParamBlocks);
			bs_deleteN(mTextures, mNumTextures);
			bs_deleteN(mSamplerStates, mNumSamplerStates);
		}

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
		void setParamBlockBuffer(UINT32 slot, const TParamsBuffer& paramBlockBuffer)
		{
			if (slot < 0 || slot >= mNumParamBlocks)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
			}

			mParamBlockBuffers[slot] = paramBlockBuffer;

			_markCoreDirty();
		}

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
		void setParamBlockBuffer(const String& name, const TParamsBuffer& paramBlockBuffer)
		{
			auto iterFind = mParamDesc->paramBlocks.find(name);

			if (iterFind == mParamDesc->paramBlocks.end())
			{
				LOGWRN("Cannot find parameter block with the name: " + name);
				return;
			}

			mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;

			_markCoreDirty();
		}

		/**
		* @brief	Returns a handle for the parameter with the specified name.
		*			Handle may then be stored and used for quickly setting or retrieving
		*			values to/from that parameter.
		*
		*			Throws exception if parameter with that name and type doesn't exist.
		*
		*			Parameter handles will be invalidated when their parent GpuParams object changes.
		*/
		template<class T> void getParam(const String& name, TGpuDataParam<T, Core>& output) const
		{
			BS_EXCEPT(InvalidParametersException, "Unsupported parameter type");
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<float>(const String& name, TGpuDataParam<float, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT1)
			{
				output = TGpuDataParam<float, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find float parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<float, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Vector2>(const String& name, TGpuDataParam<Vector2, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT2)
			{
				output = TGpuDataParam<Vector2, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find vector (2) parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Vector2, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Vector3>(const String& name, TGpuDataParam<Vector3, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT3)
			{
				output = TGpuDataParam<Vector3, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find vector (3) parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Vector3, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Vector4>(const String& name, TGpuDataParam<Vector4, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT4)
			{
				output = TGpuDataParam<Vector4, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find vector (4) parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Vector4, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Color>(const String& name, TGpuDataParam<Color, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT4)
			{
				output = TGpuDataParam<Color, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find color parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Color, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Matrix3>(const String& name, TGpuDataParam<Matrix3, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_MATRIX_3X3)
			{
				output = TGpuDataParam<Matrix3, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find matrix (3x3) parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Matrix3, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		* @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		*/
		template<>
		void getParam<Matrix4>(const String& name, TGpuDataParam<Matrix4, Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_MATRIX_4X4)
			{
				output = TGpuDataParam<Matrix4, Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find matrix (4x4) parameter with the name '" + name + "'");
			}
			else
				output = TGpuDataParam<Matrix4, Core>(&iterFind->second, _getThisPtr());
		}

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getStructParam(const String& name, TGpuParamStruct<Core>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_STRUCT)
			{
				output = TGpuParamStruct<Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find struct parameter with the name '" + name + "'");
			}
			else
				output = TGpuParamStruct<Core>(&iterFind->second, _getThisPtr());
		}

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getTextureParam(const String& name, TGpuParamTexture<Core>& output) const
		{
			auto iterFind = mParamDesc->textures.find(name);

			if (iterFind == mParamDesc->textures.end())
			{
				output = TGpuParamTexture<Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find texture parameter with the name '" + name + "'");
			}
			else
				output = TGpuParamTexture<Core>(&iterFind->second, _getThisPtr());
		}

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getLoadStoreTextureParam(const String& name, TGpuParamLoadStoreTexture<Core>& output) const
		{
			auto iterFind = mParamDesc->textures.find(name);

			if (iterFind == mParamDesc->textures.end())
			{
				output = TGpuParamLoadStoreTexture<Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find texture parameter with the name '" + name + "'");
			}
			else
				output = TGpuParamLoadStoreTexture<Core>(&iterFind->second, _getThisPtr());
		}

		/**
		 * @copydoc	getParam(const String&, TGpuDataParam<T, Core>&)
		 */
		void getSamplerStateParam(const String& name, TGpuParamSampState<Core>& output) const
		{
			auto iterFind = mParamDesc->samplers.find(name);

			if (iterFind == mParamDesc->samplers.end())
			{
				output = TGpuParamSampState<Core>(&iterFind->second, nullptr);
				LOGWRN("Cannot find sampler state parameter with the name '" + name + "'");
			}
			else
				output = TGpuParamSampState<Core>(&iterFind->second, _getThisPtr());
		}

		/**
		 * @brief	Gets a parameter block buffer from the specified slot.
		 */
		TParamsBuffer getParamBlockBuffer(UINT32 slot) const
		{
			if (slot < 0 || slot >= mNumParamBlocks)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
			}

			return mParamBlockBuffers[slot];
		}

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		TTexture getTexture(UINT32 slot)
		{
			if (slot < 0 || slot >= mNumTextures)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumTextures - 1) + ". Requested: " + toString(slot));
			}

			return mTextures[slot];
		}

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		TSampler getSamplerState(UINT32 slot)
		{
			if (slot < 0 || slot >= mNumSamplerStates)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
			}

			return mSamplerStates[slot];
		}

		/**
		 * @brief	Sets a texture at the specified slot.
		 */
		void setTexture(UINT32 slot, const TTexture& texture)
		{
			if (slot < 0 || slot >= mNumTextures)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumTextures - 1) + ". Requested: " + toString(slot));
			}

			mTextures[slot] = texture;
			_markCoreDirty();
		}

		/**
		 * @brief	Sets a sampler state at the specified slot.
		 */
		void setSamplerState(UINT32 slot, const TSampler& sampler)
		{
			if (slot < 0 || slot >= mNumSamplerStates)
			{
				BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
					toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
			}

			mSamplerStates[slot] = sampler;
			_markCoreDirty();
		}

	protected:
		/**
		 * @copydoc	CoreObject::getThisPtr
		 */
		SPtr<typename TGpuParamsType<Core>::Type> _getThisPtr() const
		{
			return std::static_pointer_cast<typename TGpuParamsType<Core>::Type>(getThisPtr());
		}

		TParamsBuffer* mParamBlockBuffers;
		TTexture* mTextures;
		TSampler* mSamplerStates;
	};

	/**
	 * @brief	Core thread version of GpuParams.
	 *
	 * @see		GpuParams
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuParamsCore : public TGpuParams<SPtr<GpuParamBlockBufferCore>, SPtr<TextureCore>, SPtr<SamplerStateCore>, true>
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
	class BS_CORE_EXPORT GpuParams : public TGpuParams<GpuParamBlockBufferPtr, HTexture, HSamplerState, false>
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
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;
	};
}