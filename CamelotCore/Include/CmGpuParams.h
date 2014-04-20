#pragma once

#include "CmPrerequisites.h"
#include "CmGpuParam.h"
#include "CmBindableGpuParams.h"

namespace BansheeEngine
{
	class CM_EXPORT GpuParams
	{
	public:
		GpuParams(GpuParamDesc& paramDesc, bool transposeMatrices);
		~GpuParams();

		void setParamBlockBuffer(UINT32 slot, const GpuParamBlockBufferPtr& paramBlockBuffer);
		void setParamBlockBuffer(const String& name, const GpuParamBlockBufferPtr& paramBlockBuffer);

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

		void getStructParam(const String& name, GpuParamStruct& output) const;
		void getTextureParam(const String& name, GpuParamTexture& output) const;
		void getSamplerStateParam(const String& name, GpuParamSampState& output) const;

	private:
		friend class BindableGpuParams;

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
		mutable Map<String, GpuParamStruct>::type mStructParams;
		mutable Map<String, GpuParamTexture>::type mTextureParams;
		mutable Map<String, GpuParamSampState>::type mSampStateParams;
	};
}