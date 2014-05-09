#include "CmRenderSystemCapabilities.h"
#include "CmException.h"
#include "CmString.h"

namespace BansheeEngine 
{
	Vector<BansheeEngine::String> RenderSystemCapabilities::msGPUVendorStrings;

	RenderSystemCapabilities::RenderSystemCapabilities()
		: mVendor(GPU_UNKNOWN)
		, mNumWorldMatrices(0)
		, mNumCombinedTextureUnits(0)
		, mNumCombinedUniformBlocks(0)
		, mStencilBufferBitDepth(0)
		, mNumVertexBlendMatrices(0)
		, mNumMultiRenderTargets(1)
		, mMaxBoundVertexBuffers(1)
		, mNonPOW2TexturesLimited(false)
	{

		for(int i = 0; i < CAPS_CATEGORY_COUNT; i++)
		{
			mCapabilities[i] = 0;
		}
		mCategoryRelevant[CAPS_CATEGORY_COMMON] = true;
		mCategoryRelevant[CAPS_CATEGORY_COMMON_2] = true;

		mCategoryRelevant[CAPS_CATEGORY_D3D11] = false;
		mCategoryRelevant[CAPS_CATEGORY_D3D9] = false;
		mCategoryRelevant[CAPS_CATEGORY_GL] = false;

		mNumTextureUnitsPerStage[GPT_VERTEX_PROGRAM] = 0;
		mNumTextureUnitsPerStage[GPT_FRAGMENT_PROGRAM] = 0;
		mNumTextureUnitsPerStage[GPT_GEOMETRY_PROGRAM] = 0;
		mNumTextureUnitsPerStage[GPT_HULL_PROGRAM] = 0;
		mNumTextureUnitsPerStage[GPT_DOMAIN_PROGRAM] = 0;
		mNumTextureUnitsPerStage[GPT_COMPUTE_PROGRAM] = 0;

		mNumUniformBlocksPerStage[GPT_VERTEX_PROGRAM] = 0;
		mNumUniformBlocksPerStage[GPT_FRAGMENT_PROGRAM] = 0;
		mNumUniformBlocksPerStage[GPT_GEOMETRY_PROGRAM] = 0;
		mNumUniformBlocksPerStage[GPT_HULL_PROGRAM] = 0;
		mNumUniformBlocksPerStage[GPT_DOMAIN_PROGRAM] = 0;
		mNumUniformBlocksPerStage[GPT_COMPUTE_PROGRAM] = 0;
	}

	RenderSystemCapabilities::~RenderSystemCapabilities()
	{
	}

	GPUVendor RenderSystemCapabilities::vendorFromString(const String& vendorString)
	{
		initVendorStrings();
		GPUVendor ret = GPU_UNKNOWN;
		String cmpString = vendorString;
		StringUtil::toLowerCase(cmpString);
		for (int i = 0; i < GPU_VENDOR_COUNT; ++i)
		{
			// case insensitive (lower case)
			if (msGPUVendorStrings[i] == cmpString)
			{
				ret = static_cast<GPUVendor>(i);
				break;
			}
		}

		return ret;
		
	}

	String RenderSystemCapabilities::vendorToString(GPUVendor v)
	{
		initVendorStrings();
		return msGPUVendorStrings[v];
	}

	void RenderSystemCapabilities::initVendorStrings()
	{
		if (msGPUVendorStrings.empty())
		{
			// Always lower case
			msGPUVendorStrings.resize(GPU_VENDOR_COUNT);
			msGPUVendorStrings[GPU_UNKNOWN] = "unknown";
			msGPUVendorStrings[GPU_NVIDIA] = "nvidia";
			msGPUVendorStrings[GPU_AMD] = "amd";
			msGPUVendorStrings[GPU_INTEL] = "intel";
		}
	}
}
