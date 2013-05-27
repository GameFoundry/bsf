#pragma once

#include <vector>
#include <cmath>

namespace CamelotFramework
{
	/**
	 * @brief	Memory block pool.
	 *
	 * @tparam	PageSizePow2	2^PageSizePow2 is the size of one page in the pool. This size divided by number of blocks per chunk
	 * 							determines the maximum size you can allocate in the pool. Larger allocations will use a general allocator.
	 * @tparam	BlocksPerChunkPow2 Pages are split into chunks and chunks are split into smaller chunks based on this value.
	 * 							   Higher values generally guarantee less allocations and better performance, but potentially 
	 * 							   more wasted memory. Actual blocks per chunk is calculated by 2^BlocksPerChunkPow2.
	 * 							   Has to be equal or less than PageSizePow2. Must be equal or less than 8.
	 * 							   (e.g. 0 means 1 block per chunk, 1 means 2 blocks per chunk, etc.)
	 */
	template <int PageSizePow2 = 20, int BlocksPerChunkPow2 = 5>
	class MemBlockPool
	{
		struct MemChunk
		{
			MemChunk()
				:firstFreeBlock(0), numAvailableBlocks(0), data(nullptr)
			{ }

			void init(unsigned char* _data, unsigned int blockSize, unsigned char numBlocks)
			{
				data = _data;
				numAvailableBlocks = numBlocks;
				firstFreeBlock = 0;

				unsigned char* dataPtr = data;
				for(unsigned char i = 0; i < numBlocks; ++i)
				{
					*dataPtr = i;
					dataPtr += blockSize;
				}
			}

			void release()
			{

			}

			void* alloc(unsigned int blockSize)
			{
				if(numAvailableBlocks == 0) return nullptr;

				unsigned char* result = data + (firstFreeBlock * blockSize);
				firstFreeBlock = *result;

				--numAvailableBlocks;
				return result;
			}

			void dealloc(void* dataToRls, unsigned int blockSize)
			{
				unsigned char* toRelease = static_cast<unsigned char*>(dataToRls);
				*toRelease = firstFreeBlock;
				firstFreeBlock = (toRelease - data) / blockSize;

				++numAvailableBlocks;
			}

			unsigned char* data;
			unsigned char firstFreeBlock;
			unsigned char numAvailableBlocks;
		};

		struct MemPool
		{
			std::vector<MemChunk> mChunks;
			unsigned int mBlockSize;
			MemChunk* mAllocChunk;
			MemChunk* mDeallocChunk;
		};

	public:
		MemBlockPool()
		{
			unsigned int blockSize = 1 << BlocksPerChunkPow2;
			mNumPools = PageSizePow2 - BlocksPerChunkPow2;
			for(int i = 0; i < mNumPools + 1; i++)
			{
				mPools[i].mBlockSize = blockSize;
				mPools[i].mAllocChunk = nullptr;
				mPools[i].mDeallocChunk = nullptr;
				blockSize = blockSize << 1;
			}

			mBlocksPerChunk = 1;
			for(int i = 0; i < BlocksPerChunkPow2; i++)
				mBlocksPerChunk <<= 1;
		}

		~MemBlockPool()
		{
			// TODO - Handle release

			//for(auto iter = mChunks.begin(); iter != mChunks.end(); ++iter)
			//	iter->release();
		}

		void* alloc(unsigned int size)
		{
			unsigned int poolIdx = sizeToPool(size) - BlocksPerChunkPow2;

			if(poolIdx >= mNumPools) 
				return CM_NEW_BYTES(size, GenAlloc);

			MemPool& pool = mPools[poolIdx];
			if(pool.mAllocChunk == nullptr || pool.mAllocChunk->numAvailableBlocks == 0)
			{
				for(auto iter = pool.mChunks.begin();; ++iter)
				{
					if(iter == pool.mChunks.end())
					{
						pool.mChunks.reserve(pool.mChunks.size() + 1);

						MemChunk newChunk;
						newChunk.init((unsigned char*)alloc(pool.mBlockSize * mBlocksPerChunk), pool.mBlockSize, mBlocksPerChunk);

						pool.mChunks.push_back(newChunk);
						pool.mAllocChunk = &pool.mChunks.back();
						pool.mDeallocChunk = &pool.mChunks.back();
						break;
					}

					if(iter->numAvailableBlocks > 0)
					{
						pool.mAllocChunk = &*iter;
						break;
					}
				}
			}

			return pool.mAllocChunk->alloc(pool.mBlockSize);
		}

		void dealloc(void* dataToRls, unsigned int size)
		{
			unsigned int poolIdx = sizeToPool(size) - BlocksPerChunkPow2;

			if(poolIdx >= mNumPools) 
				return CM_DELETE_BYTES(dataToRls, GenAlloc);

			MemPool& pool = mPools[poolIdx];
			if(pool.mDeallocChunk == nullptr || dataToRls < pool.mDeallocChunk->data || dataToRls >= (pool.mDeallocChunk->data + pool.mBlockSize * mBlocksPerChunk))
			{
				for(auto iter = pool.mChunks.begin();; ++iter)
				{
					assert(iter != pool.mChunks.end()); // Trying to dealloc memory that wasn't allocated by this allocator

					if(dataToRls >= iter->data && dataToRls < (iter->data + pool.mBlockSize * mBlocksPerChunk))
					{
						pool.mDeallocChunk = &*iter;
						break;
					}
				}
			}
		
			pool.mDeallocChunk->dealloc(dataToRls, pool.mBlockSize);

			// If chunk is empty, release it (either to a higher level chunk to re-use, or to the OS if the chunk is highest level)
			if(pool.mDeallocChunk->numAvailableBlocks == mBlocksPerChunk) // Chunk is completely empty
			{
				auto findIter = std::find(pool.mChunks.begin(), pool.mChunks.end(), pool.mDeallocChunk);
				pool.mChunks.erase(findIter);

				dealloc(pool.mDeallocChunk->data, pool.mBlockSize * mBlocksPerChunk);

				pool.mDeallocChunk = nullptr;
			}
		}

	private:
		MemPool mPools[PageSizePow2 + 1];
		unsigned int mBlocksPerChunk;
		unsigned int mNumPools;

		unsigned int sizeToPool(unsigned int size)
		{
			// TODO - Size cannot be zero

			unsigned int targetlevel = 0;
			while (size >>= 1) // I can speed this up using a BSR instruction, in case compiler doesn't already do it
				++targetlevel;

			return targetlevel;
		}
	};
}