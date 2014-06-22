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
		const String& getLanguage() const;

		/**
		 * @copydoc	GpuProgramFactory::getLanguage(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, const Vector<HGpuProgInclude>*, bool)
		 */
		GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacencyInfo);

		/**
		* @copydoc	GpuProgramFactory::create(GpuProgramType)
		*/
		GpuProgramPtr create(GpuProgramType type);

	protected:
		static const String LANGUAGE_NAME;
	};
}