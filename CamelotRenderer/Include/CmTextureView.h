#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	struct CM_EXPORT TEXTURE_VIEW_DESC
	{
		UINT32 mostDetailMip;
		UINT32 numMips;
		UINT32 firstArraySlice;
		UINT32 numArraySlices;
		GpuViewUsage usage;
	};

	class CM_EXPORT TextureView
	{
	public:
		class HashFunction
		{
		public:
			size_t operator()(const TEXTURE_VIEW_DESC &key) const;
		};

		class EqualFunction
		{
		public:
			bool operator()(const TEXTURE_VIEW_DESC &a, const TEXTURE_VIEW_DESC &b) const;
		};

		TextureView(Texture* texture, TEXTURE_VIEW_DESC& _desc);
		virtual ~TextureView();

		const TEXTURE_VIEW_DESC& getDesc() const { return mDesc; }

	protected:
		TEXTURE_VIEW_DESC mDesc;
	};
}