//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	struct GpuParamsInternalData;

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
	class BS_CORE_EXPORT GpuParams
	{
		struct PrivatelyConstruct {};
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
		GpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices);

		/**
		 * @brief	Private constructor for internal use. Performs no initialization.
		 */
		GpuParams(const GpuParamDescPtr& paramDesc, PrivatelyConstruct& dummy);

		~GpuParams();

		// Note: Disallow copy/assign because it would require some care when copying (copy internal data shared_ptr and
		// all the internal buffers too). Trivial to implement but not needed at this time. Un-delete and implement if necessary.
		GpuParams(const GpuParams& other) = delete;
		GpuParams& operator=(const GpuParams& rhs) = delete;

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
		void setParamBlockBuffer(UINT32 slot, const GpuParamBlockBufferPtr& paramBlockBuffer);

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
		void setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer);

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
		 * @brief	Returns a handle for the parameter with the specified name. 
		 *			Handle may then be stored and used for quickly setting or retrieving
		 *			values to/from that parameter.
		 *
		 *			Throws exception if parameter with that name and type doesn't exist.
		 *
		 *			Parameter handles will be invalidated when their parent GpuParams object changes.
		 */
		template<class T> void getParam(const String& name, TGpuDataParam<T>& output) const
		{
			BS_EXCEPT(InvalidParametersException, "Unsupported parameter type");
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<float>(const String& name, TGpuDataParam<float>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT1)
				BS_EXCEPT(InvalidParametersException, "Cannot find float parameter with the name '" + name + "'");

			output = GpuParamFloat(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<Vector2>(const String& name, TGpuDataParam<Vector2>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT2)
				BS_EXCEPT(InvalidParametersException, "Cannot find vector (2) parameter with the name '" + name + "'");

			output = GpuParamVec2(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<Vector3>(const String& name, TGpuDataParam<Vector3>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT3)
				BS_EXCEPT(InvalidParametersException, "Cannot find vector (3) parameter with the name '" + name + "'");

			output = GpuParamVec3(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<Vector4>(const String& name, TGpuDataParam<Vector4>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_FLOAT4)
				BS_EXCEPT(InvalidParametersException, "Cannot find vector (4) parameter with the name '" + name + "'");

			output = GpuParamVec4(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<Matrix3>(const String& name, TGpuDataParam<Matrix3>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_MATRIX_3X3)
				BS_EXCEPT(InvalidParametersException, "Cannot find matrix (3x3) parameter with the name '" + name + "'");

			output = GpuParamMat3(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<Matrix4>(const String& name, TGpuDataParam<Matrix4>& output) const
		{
			auto iterFind = mParamDesc->params.find(name);

			if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_MATRIX_4X4)
				BS_EXCEPT(InvalidParametersException, "Cannot find matrix (4x4) parameter with the name '" + name + "'");

			output = GpuParamMat4(&iterFind->second, mInternalData);
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		void getStructParam(const String& name, GpuParamStruct& output) const;

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		void getTextureParam(const String& name, GpuParamTexture& output) const;

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		void getSamplerStateParam(const String& name, GpuParamSampState& output) const;

		/**
		 * @brief	Uploads all CPU stored parameter buffer data to the GPU buffers.
		 *
		 * @note	Core thread only.
		 */
		void updateHardwareBuffers();

		/**
		 * @brief	Gets a parameter block buffer from the specified slot.
		 */
		GpuParamBlockBufferPtr getParamBlockBuffer(UINT32 slot) const;

		/**
		 * @brief	Gets a texture bound to the specified slot.
		 */
		HTexture getTexture(UINT32 slot);

		/**
		 * @brief	Gets a sampler state bound to the specified slot.
		 */
		HSamplerState getSamplerState(UINT32 slot);

		/**
		 * @brief	Returns an exact copy of this object. Internal parameter buffers will also be cloned,
		 *			but should only be used on the core thread.
		 *
		 * @note	Optional frame allocator to allocate the returned data with. If not specified
		 *			allocation will be done using normal means.
		 *			Internal method.
		 */
		GpuParamsPtr _cloneForCore(FrameAlloc* frameAlloc = nullptr) const;

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know  when internal data has changed and core proxy potentially needs to be updated.
		 *
		 * @note	Internal method. Sim thread only.
		 */
		bool _isCoreDirty() const;

		/**
		 * @brief	Marks the core dirty flag as clean.
		 *
		 * @note	Internal method. Sim thread only.
		 */
		void _markCoreClean();

	private:
		GpuParamDescPtr mParamDesc;
		std::shared_ptr<GpuParamsInternalData> mInternalData;

		/**
		 * @brief	Gets a descriptor for a data parameter with the specified name.
		 */
		GpuParamDataDesc* getParamDesc(const String& name) const;

		/**
		 * @brief	Allocates and constructs internal buffers. Parameter counts must have been previously assigned.
		 *			If provided, internal data will be allocated using the frame allocator, otherwise using
		 *			the normal allocator.
		 */
		void constructInternalBuffers(FrameAlloc* frameAlloc = nullptr);

		/**
		 * @brief	Marks the core data as dirty, signifying the core thread it should update it.
		 */
		void markCoreDirty();
	};

	/**
	 * @brief	Structure used for storing GpuParams internal data.
	 */
	struct GpuParamsInternalData
	{
		GpuParamsInternalData();

		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		GpuParamBlockPtr* mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;

		bool mTransposeMatrices;
		bool mIsDestroyed;
		UINT32 mCoreDirtyFlags;

		FrameAlloc* mFrameAlloc;
	};
}