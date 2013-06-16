#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuParams
	{
	public:
		GpuParams(GpuParamDesc& paramDesc);
		~GpuParams();

		GpuParamBlockBufferPtr getParamBlockBuffer(UINT32 slot) const;
		GpuParamBlockBufferPtr getParamBlockBuffer(const String& name) const;

		void setParamBlockBuffer(UINT32 slot, GpuParamBlockBufferPtr paramBlockBuffer);
		void setParamBlockBuffer(const String& name, GpuParamBlockBufferPtr paramBlockBuffer);

		const GpuParamDesc& getParamDesc() const { return mParamDesc; }
		UINT32 getDataParamSize(const String& name) const;

		bool hasParam(const String& name) const;
		bool hasTexture(const String& name) const;
		bool hasSamplerState(const String& name) const;
		bool hasParamBlock(const String& name) const;

		void setParam(const String& name, float value, UINT32 arrayIndex = 0);
		void setParam(const String& name, int value, UINT32 arrayIndex = 0);
		void setParam(const String& name, bool value, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Vector4& vec, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Vector3& vec, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Vector2& vec, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Matrix4& mat, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Matrix3& mat, UINT32 arrayIndex = 0);
		void setParam(const String& name, const Color& color, UINT32 arrayIndex = 0);

		/**
		 * @brief	Sets a parameter.
		 *
		 * @param	name	  	Name of the parameter.
		 * @param	value	  	Parameter data.
		 * @param	size	  	Size of the provided data. It can be exact size or lower than the exact size of the wanted field.
		 * 						If it's lower unused bytes will be set to 0. 
		 * @param	arrayIndex	(optional) zero-based index of the array.
		 */
		void setParam(const String& name, const void* value, UINT32 sizeBytes, UINT32 arrayIndex = 0);

		void setTexture(const String& name, const HTexture& val);
		HTexture getTexture(UINT32 slot);

		void setSamplerState(const String& name, const HSamplerState& val);
		HSamplerState getSamplerState(UINT32 slot);

		void setTransposeMatrices(bool transpose) { mTransposeMatrices = transpose; }

		/**
		 * @brief	Updates all used hardware parameter buffers. Should ONLY be called from core thread.
		 */
		void updateHardwareBuffers();

		/**
		 * @brief	Creates the copy of this object in a special way. Should only be called
		 * 			internally by deferred render context when passing gpu params to the core thread.
		 */
		static BindableGpuParams createBindableCopy(GpuParamsPtr params);

		/**
		 * @brief	Needs to be called on any copy created with "createCopyForDeferred" before the object is deleted.
		 */
		static void releaseBindableCopy(BindableGpuParams& bindableParams);
	private:
		GpuParamDesc& mParamDesc;
		bool mTransposeMatrices;

		GpuParamDataDesc* getParamDesc(const String& name) const;

		Vector<GpuParamBlock*>::type mParamBlocks;
		Vector<GpuParamBlockBufferPtr>::type mParamBlockBuffers;
		Vector<HTexture>::type mTextures;
		Vector<HSamplerState>::type mSamplerStates;
	};

	/**
	 * @brief	Specialized class for binding GPU parameters to the render system. You should not
	 * 			handle this class manually.
	 * 			
	 * @note	Upon assignment this class transfers ownership of its data. Internal data
	 * 			is destroyed when last assigned instance goes out of scope.
	 * 			(In short, you should never have more than one active copy of an instance of this class)
	 */
	class CM_EXPORT BindableGpuParams
	{
	public:
		BindableGpuParams(const BindableGpuParams& source);
		~BindableGpuParams();

		GpuParams& getParams() const { return *mParams; }

	private:
		friend class GpuParams;

		BindableGpuParams(GpuParams* params);

		GpuParams *mParams;
		mutable bool mIsDataOwner;
	};
}