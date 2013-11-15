#pragma once

#include "CmPrerequisites.h"
#include "CmGpuParam.h"
#include "CmBindableGpuParams.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuParams
	{
	public:
		GpuParams(GpuParamDesc& paramDesc, bool transposeMatrices);
		~GpuParams();

		void setParamBlockBuffer(UINT32 slot, GpuParamBlockBufferPtr paramBlockBuffer);
		void setParamBlockBuffer(const String& name, GpuParamBlockBufferPtr paramBlockBuffer);

		const GpuParamDesc& getParamDesc() const { return mParamDesc; }
		UINT32 getDataParamSize(const String& name) const;

		bool hasParam(const String& name) const;
		bool hasTexture(const String& name) const;
		bool hasSamplerState(const String& name) const;
		bool hasParamBlock(const String& name) const;

		template<class T> void getParam(const String& name, GpuDataParamBase<T>& output) const
		{
			CM_EXCEPT(InvalidParametersException, "Unsupported parameter type");
		}

		template<>
		void getParam<float>(const String& name, GpuDataParamBase<float>& output) const
		{
			auto iterFind = mFloatParams.find(name);

			if(iterFind == mFloatParams.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find float parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		template<>
		void getParam<Vector2>(const String& name, GpuDataParamBase<Vector2>& output) const
		{
			auto iterFind = mVec2Params.find(name);

			if(iterFind == mVec2Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(2) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		template<>
		void getParam<Vector3>(const String& name, GpuDataParamBase<Vector3>& output) const
		{
			auto iterFind = mVec3Params.find(name);

			if(iterFind == mVec3Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(3) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		template<>
		void getParam<Vector4>(const String& name, GpuDataParamBase<Vector4>& output) const
		{
			auto iterFind = mVec4Params.find(name);

			if(iterFind == mVec4Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find vector(4) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		template<>
		void getParam<Matrix3>(const String& name, GpuDataParamBase<Matrix3>& output) const
		{
			auto iterFind = mMat3Params.find(name);

			if(iterFind == mMat3Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find matrix(3x3) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

		template<>
		void getParam<Matrix4>(const String& name, GpuDataParamBase<Matrix4>& output) const
		{
			auto iterFind = mMat4Params.find(name);

			if(iterFind == mMat4Params.end())
				CM_EXCEPT(InvalidParametersException, "Cannot find matrix(4x4) parameter with the name '" + name + "'");

			output = iterFind->second;
		}

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

		mutable Map<String, GpuParamFloat>::type mFloatParams;
		mutable Map<String, GpuParamVec2>::type mVec2Params;
		mutable Map<String, GpuParamVec3>::type mVec3Params;
		mutable Map<String, GpuParamVec4>::type mVec4Params;
		mutable Map<String, GpuParamMat3>::type mMat3Params;
		mutable Map<String, GpuParamMat4>::type mMat4Params;
	};
}