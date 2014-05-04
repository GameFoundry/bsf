#pragma once

#include "CmPrerequisites.h"
#include "CmGpuParam.h"
#include "CmBindableGpuParams.h"

namespace BansheeEngine
{
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
	class CM_EXPORT GpuParams
	{
	public:
		/**
		 * @brief	Creates new GpuParams object using the specified parameter descriptions.
		 *
		 * @param	transposeMatrices	If true the stored matrices will be transposed before
		 *								submitted to the GPU (some APIs require different
		 *								matrix layout).
		 *
		 * @note	You normally do not want to call this manually. Instead use GpuProgram::createParameters.
		 */
		GpuParams(GpuParamDesc& paramDesc, bool transposeMatrices);
		~GpuParams();

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
		* @note		This is useful if you want to share a parameter buffer among multiple GPU programs.
		*			You would only set the values once and then share the buffer among all other GpuParams.
		*
		*			It is up to the caller to guarantee the provided buffer matches parameter block
		*			descriptor for this slot.
		*/
		void setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer);

		/**
		 * @brief	Returns a description of all stored parameters.
		 */
		const GpuParamDesc& getParamDesc() const { return mParamDesc; }

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
		template<class T> void getParam(const String& name, GpuDataParamBase<T>& output) const
		{
			CM_EXCEPT(InvalidParametersException, "Unsupported parameter type");
		}

		/**
		 * @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		 */
		template<>
		void getParam<float>(const String& name, GpuDataParamBase<float>& output) const
		{
			auto iterFind = mFloatParams.find(name);

			if(iterFind == mFloatParams.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find float parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		/**
		* @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		*/
		template<>
		void getParam<Vector2>(const String& name, GpuDataParamBase<Vector2>& output) const
		{
			auto iterFind = mVec2Params.find(name);

			if(iterFind == mVec2Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(2) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		/**
		* @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		*/
		template<>
		void getParam<Vector3>(const String& name, GpuDataParamBase<Vector3>& output) const
		{
			auto iterFind = mVec3Params.find(name);

			if(iterFind == mVec3Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(3) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		/**
		* @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		*/
		template<>
		void getParam<Vector4>(const String& name, GpuDataParamBase<Vector4>& output) const
		{
			auto iterFind = mVec4Params.find(name);

			if(iterFind == mVec4Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(4) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		/**
		* @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		*/
		template<>
		void getParam<Matrix3>(const String& name, GpuDataParamBase<Matrix3>& output) const
		{
			auto iterFind = mMat3Params.find(name);

			if(iterFind == mMat3Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find matrix(3x3) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		/**
		* @copydoc	getParam(const String&, GpuDataParamBase<T>&)
		*/
		template<>
		void getParam<Matrix4>(const String& name, GpuDataParamBase<Matrix4>& output) const
		{
			auto iterFind = mMat4Params.find(name);

			if(iterFind == mMat4Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find matrix(4x4) parameter with the name '" + name + "'");

			output = iterFind->second;
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

	private:
		friend class BindableGpuParams;

		GpuParamDesc& mParamDesc;
		bool mTransposeMatrices;

		/**
		 * @brief	Gets a descriptor for a data parameter with the specified name.
		 */
		GpuParamDataDesc* getParamDesc(const String& name) const;

		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		GpuParamBlock** mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;

		mutable Map<String, GpuParamFloat>::type mFloatParams;
		mutable Map<String, GpuParamVec2>::type mVec2Params;
		mutable Map<String, GpuParamVec3>::type mVec3Params;
		mutable Map<String, GpuParamVec4>::type mVec4Params;
		mutable Map<String, GpuParamMat3>::type mMat3Params;
		mutable Map<String, GpuParamMat4>::type mMat4Params;
		mutable Map<String, GpuParamStruct>::type mStructParams;
		mutable Map<String, GpuParamTexture>::type mTextureParams;
		mutable Map<String, GpuParamSampState>::type mSampStateParams;
	};
}