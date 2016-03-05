//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuProgramManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 11 HLSL GPU programs.
	 */
	class BS_D3D11_EXPORT D3D11HLSLProgramFactory : public GpuProgramFactory
	{
	public:
		D3D11HLSLProgramFactory();
		~D3D11HLSLProgramFactory();

		/**
		 * @copydoc	GpuProgramFactory::getLanguage
		 */
		const String& getLanguage() const override;

		/**
		 * @copydoc	GpuProgramFactory::create(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, bool)
		 */
		SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool requireAdjacencyInfo) override;

		/**
		* @copydoc	GpuProgramFactory::create(GpuProgramType)
		*/
		SPtr<GpuProgramCore> create(GpuProgramType type) override;

	protected:
		static const String LANGUAGE_NAME;
	};
}