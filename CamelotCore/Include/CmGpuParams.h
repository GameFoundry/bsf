#pragma once

#include "CmPrerequisites.h"
#include "CmBindableGpuParams.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuParams
	{
	public:
		GpuParams(GpuParamDesc& paramDesc);
		~GpuParams();

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
		void setSamplerState(const String& name, const HSamplerState& val);
		
		void setTransposeMatrices(bool transpose) { mTransposeMatrices = transpose; }

		/**
		 * @brief	Creates the copy of this object in a special way. Should only be called
		 * 			internally by core thread accessor when passing gpu params to the core thread.
		 */
		static BindableGpuParams createBindableCopy(const GpuParamsPtr& params);

	private:
		GpuParamDesc& mParamDesc;
		bool mTransposeMatrices;

		GpuParamDataDesc* getParamDesc(const String& name) const;

		UINT8* mData;

		UINT32 mNumParamBlocks;
		UINT32 mNumTextures;
		UINT32 mNumSamplerStates;

		GpuParamBlock** mParamBlocks;
		GpuParamBlockBufferPtr* mParamBlockBuffers;
		HTexture* mTextures;
		HSamplerState* mSamplerStates;
	};
}