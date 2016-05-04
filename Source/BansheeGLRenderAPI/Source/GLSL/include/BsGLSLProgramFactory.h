//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuProgramManager.h"

namespace BansheeEngine
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	Factory class that deals with creating GLSL GPU programs. */
    class BS_RSGL_EXPORT GLSLProgramFactory : public GpuProgramFactory
    {
    public:
		/** @copydoc GpuProgramFactory::getLanguage */
		const String& getLanguage() const override;

		/**
		 * @copydoc	GpuProgramFactory::getLanguage(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, bool)
		 */
        SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool requireAdjacency) override;

		/** @copydoc GpuProgramFactory::create(GpuProgramType) */
		SPtr<GpuProgramCore> create(GpuProgramType type) override;

	protected:
		static const String LANGUAGE_NAME;
    };

	/** @} */
}