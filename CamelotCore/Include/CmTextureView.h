#pragma once

#include "CmPrerequisites.h"
#include "CmCoreObject.h"
#include "CmCommonEnums.h"

namespace CamelotFramework
{
	struct CM_EXPORT TEXTURE_VIEW_DESC
	{
		UINT32 mostDetailMip;
		UINT32 numMips;
		UINT32 firstArraySlice;
		UINT32 numArraySlices;
		GpuViewUsage usage;
	};

	class CM_EXPORT TextureView : public CoreObject
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

		virtual ~TextureView();

		virtual void initialize(TexturePtr texture, TEXTURE_VIEW_DESC& _desc);

		UINT32 getMostDetailedMip() const { return mDesc.mostDetailMip; }
		UINT32 getNumMips() const { return mDesc.numMips; }
		UINT32 getFirstArraySlice() const { return mDesc.firstArraySlice; }
		UINT32 getNumArraySlices() const { return mDesc.numArraySlices; }
		GpuViewUsage getUsage() const { return mDesc.usage; }

		const TEXTURE_VIEW_DESC& getDesc() const { return mDesc; }
		TexturePtr getTexture() const { return mOwnerTexture; }

	protected:
		friend class Texture;

		TEXTURE_VIEW_DESC mDesc;
		TexturePtr mOwnerTexture;

		TextureView();
	};
}