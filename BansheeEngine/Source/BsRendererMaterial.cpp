#include "BsRendererMaterial.h"
#include "BsMaterial.h"

namespace BansheeEngine
{
	void RendererMaterialBase::_initialize(const SPtr<ShaderCore>& shader)
	{
		mMaterial = MaterialCore::create(shader);

		initialize();
	}
}