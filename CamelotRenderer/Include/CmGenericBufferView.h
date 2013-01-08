#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT GenericBufferView
	{
	public:
		struct Key
		{
			Key()
			{ }

			Key(const GenericBufferView& view)
				: mFirstElement(view.mFirstElement), mElementWidth(view.mElementWidth)
				, mNumElements(view.mNumElements), mRandomGpuWrite(view.mRandomGpuWrite)
			{ }

			Key(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
				: mFirstElement(firstElement), mElementWidth(elementWidth)
				, mNumElements(numElements), mRandomGpuWrite(randomGpuWrite)
			{ }

			UINT32 mFirstElement;
			UINT32 mElementWidth;
			UINT32 mNumElements;
			bool mRandomGpuWrite;
		};

		class HashFunction
		{
		public:
			size_t operator()(const Key &key) const;
		};

		class EqualFunction
		{
		public:
			bool operator()(const Key &a, const Key &b) const;
		};

		GenericBufferView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite);
		virtual ~GenericBufferView();

	private:
		UINT32 mFirstElement;
		UINT32 mElementWidth;
		UINT32 mNumElements;
		bool mRandomGpuWrite;
	};
}