//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Data describing a texture view.
	 */
	struct BS_CORE_EXPORT TEXTURE_VIEW_DESC
	{
		/**
		 * First mip level of the parent texture the view binds (0 - base level). 
		 * This applied to all array slices specified below.
		 */
		UINT32 mostDetailMip;

		/**
		 * Number of mip levels to bind to the view. 
		 * This applied to all array slices specified below.
		 */
		UINT32 numMips;

		/**
		 * First array slice the view binds to. This will be array index for 
		 * 1D and 2D array textures, texture slice index for 3D textures, and 
		 * face index for cube textures(cube index * 6).
 		 */
		UINT32 firstArraySlice;

		/**
		 * Number of array slices to bind tot he view. This will be number of
		 * array elements for 1D and 2D array textures, number of slices for 3D textures,
		 * and number of cubes for cube textures.
		 */
		UINT32 numArraySlices;

		/**
		 *	Type of texture view.
		 */
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
	class BS_CORE_EXPORT TextureView : public CoreObject
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