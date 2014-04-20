#pragma once

#include "CmPrerequisites.h"
#include "CmIndexBuffer.h"

namespace BansheeEngine 
{
	/** Summary class collecting together index data source information. */
	class CM_EXPORT IndexData
	{
    public:
        IndexData();
        ~IndexData();
		/// pointer to the HardwareIndexBuffer to use, must be specified if useIndexes = true
		IndexBufferPtr indexBuffer;

		/// index in the buffer to start from for this operation
		UINT32 indexStart;

		/// The number of indexes to use from the buffer
		UINT32 indexCount;

		/** Re-order the indexes in this index data structure to be more
			vertex cache friendly; that is to re-use the same vertices as close
			together as possible. 
		@remarks
			Can only be used for index data which consists of triangle lists.
			It would in fact be pointless to use it on triangle strips or fans
			in any case.
		*/
		void optimiseVertexCacheTriList(void);

	protected:
		/// Protected copy constructor, to prevent misuse
		IndexData(const IndexData& rhs); /* do nothing, should not use */
		/// Protected operator=, to prevent misuse
		IndexData& operator=(const IndexData& rhs); /* do not use */
	};
}