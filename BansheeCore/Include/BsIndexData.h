#pragma once

#include "BsCorePrerequisites.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Container that holds data about indices in a subset of an index buffer.
	 */
	class BS_CORE_EXPORT IndexData
	{
    public:
        IndexData();
        ~IndexData();

		IndexBufferPtr indexBuffer; /**< Reference to an index buffer to fetch the indices from. */

		UINT32 indexStart; /**< Starting index of the subset. */
		UINT32 indexCount; /**< Number of indices in the subset. */
	};
}