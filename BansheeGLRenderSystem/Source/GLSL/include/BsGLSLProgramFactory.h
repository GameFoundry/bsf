#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuProgramManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Factory class that deals with creating GLSL GPU programs.
	 */
    class BS_RSGL_EXPORT GLSLProgramFactory : public GpuProgramFactory
    {
    public:
		/**
		 * @copydoc	GpuProgramFactory::getLanguage
		 */
		const String& getLanguage() const override;

		/**
		 * @copydoc	GpuProgramFactory::getLanguage(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, bool)
		 */
        SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool requireAdjacency) override;

		/**
		 * @copydoc	GpuProgramFactory::create(GpuProgramType)
		 */
		SPtr<GpuProgramCore> create(GpuProgramType type) override;

	protected:
		static const String LANGUAGE_NAME;
    };
}