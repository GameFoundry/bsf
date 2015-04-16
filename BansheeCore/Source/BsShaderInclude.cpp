#include "BsShaderInclude.h"
#include "BsResources.h"
#include "BsShaderIncludeRTTI.h"

namespace BansheeEngine
{
	ShaderInclude::ShaderInclude(const String& includeString)
		:Resource(false), mString(includeString)
	{

	}

	HShaderInclude ShaderInclude::create(const String& includeString)
	{
		return static_resource_cast<ShaderInclude>(gResources()._createResourceHandle(_createPtr(includeString)));
	}

	ShaderIncludePtr ShaderInclude::_createPtr(const String& includeString)
	{
		ShaderIncludePtr shaderIncludePtr = bs_core_ptr<ShaderInclude, PoolAlloc>(
			new (bs_alloc<ShaderInclude, PoolAlloc>()) ShaderInclude(includeString));
		shaderIncludePtr->_setThisPtr(shaderIncludePtr);
		shaderIncludePtr->initialize();

		return shaderIncludePtr;
	}

	RTTITypeBase* ShaderInclude::getRTTIStatic()
	{
		return ShaderIncludeRTTI::instance();
	}

	RTTITypeBase* ShaderInclude::getRTTI() const
	{
		return ShaderInclude::getRTTIStatic();
	}
}