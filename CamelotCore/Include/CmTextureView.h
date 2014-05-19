#pragma once

#include "CmPrerequisites.h"
#include "CmCoreObject.h"
#include "CmCommonEnums.h"

namespace BansheeEngine
{
	/**
	 * @brief	Data describing a texture view.
	 */
	struct CM_EXPORT TEXTURE_VIEW_DESC
	{
		UINT32 mostDetailMip;
		UINT32 numMips;
		UINT32 firstArraySlice;
		UINT32 numArraySlices;
		GpuViewUsage usage;
	};

	/**
	 * @brief	Texture views allow you to reference only a party of a texture.
	 *			They may reference one or multiple mip-levels on one or multiple texture
	 *			array slices. Selected mip level will apply to all slices.
	 *
	 *			They also allow you to re-purpose a texture. (e.g. make a render target 
	 *			a bindable texture).
	 *
	 * @note	Right now texture views are only used for very specific internal purposes, 
	 *			but a more general use might come in the future.
	 */
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

		/**
		 * @brief	Returns the most detailed mip level visible by the view.
		 */
		UINT32 getMostDetailedMip() const { return mDesc.mostDetailMip; }

		/**
		 * @brief	Returns the number of mip levels in a single slice visible by the view.
		 */
		UINT32 getNumMips() const { return mDesc.numMips; }

		/**
		 * @brief	Returns the first array slice index visible by this view.
		 */
		UINT32 getFirstArraySlice() const { return mDesc.firstArraySlice; }

		/**
		 * @brief	Returns the number of array slices visible by this view.
		 */
		UINT32 getNumArraySlices() const { return mDesc.numArraySlices; }

		/**
		 * @brief	Returns texture view usage. This determines where on the pipeline can be
		 *			bind the view.
		 */
		GpuViewUsage getUsage() const { return mDesc.usage; }

		/**
		 * @brief	Returns the descriptor structure used for initializing the view.
		 */
		const TEXTURE_VIEW_DESC& getDesc() const { return mDesc; }

		/**
		 * @brief	Gets the owner texture the view is referencing.
		 */
		TexturePtr getTexture() const { return mOwnerTexture; }

	protected:
		/**
		 * @brief	Initializes the texture view. This must be called right after construction.
		 */
		virtual void initialize(TexturePtr texture, TEXTURE_VIEW_DESC& _desc);

	protected:
		friend class Texture;

		TEXTURE_VIEW_DESC mDesc;
		TexturePtr mOwnerTexture;

		TextureView();
	};
}