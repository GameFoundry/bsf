#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsGpuProgramManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of HLSL GPU programs.
	 */
    class BS_D3D9_EXPORT D3D9HLSLProgramFactory : public GpuProgramFactory
    {
    public:
        D3D9HLSLProgramFactory();
        ~D3D9HLSLProgramFactory();

		/**
		 * @copydoc	GpuProgramFactory::getLanguage
		 */
		const String& getLanguage() const;

		/**
		 * @copydoc	GpuProgramFactory::getLanguage(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, bool)
		 */
        SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool requiresAdjacency);

		/**
		 * @copydoc	GpuProgramFactory::create(GpuProgramType)
		 */
		SPtr<GpuProgramCore> create(GpuProgramType type);

	protected:
		static String LANGUAGE_NAME;
    };
}