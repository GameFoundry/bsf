//************************************ bs::framework - Copyright 2018-2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs 
{
	/** @addtogroup General
	 *  @{
	 */

	/** Declaration of DenseMap utility info. */
	template <class Type>
	struct DenseMapInfo;

	/** DenseMap iterator. */
	template<class Key, class Value,
		class KeyInfo = DenseMapInfo<Key>,
		class ValueInfo = DenseMapInfo<Value>>
		class DenseMapIterator;

	/** DenseMap constant iterator. */
	template <class Key, class Value,
		class KeyInfo = DenseMapInfo<Key>,
		class ValueInfo = DenseMapInfo<Value>>
		class DenseMapConstIterator;

	/** Utility to provide DenseMap info for implementation. */
	template <class Type>
	struct DenseMapInfo
	{
		using UINTPTR = uintptr_t;
		constexpr static Type getEmptyKey() { return static_cast<Type>(-1); }
		constexpr static Type getTombstoneKey() { return static_cast<Type>(-2); }

		constexpr static UINT32 getHashValue(const Type& val)
		{
			return (UINT32((UINTPTR)val) >> 4) ^ (UINT32((UINTPTR)val) >> 9);
		}

		constexpr static bool equal(const Type& lhs, const Type& rhs) { return lhs == rhs; }
		constexpr static bool pod() { return true; }
	};

	/** Utility to provide DenseMap info for all pointers type. */
	template <class Type>
	struct DenseMapInfo<Type*>
	{
		using UINTPTR = uintptr_t;
		constexpr static Type* getEmptyKey() { return reinterpret_cast<Type*>(-1); }
		constexpr static Type* getTombstoneKey() { return reinterpret_cast<Type*>(-2); }

		constexpr static UINT32 getHashValue(const Type* ptr)
		{
			return (UINT32((UINTPTR)ptr) >> 4) ^ (UINT32((UINTPTR)ptr) >> 9);
		}

		constexpr static bool equal(const Type* lhs, const Type* rhs) { return lhs == rhs; }
		constexpr static bool pod() { return true; }
	};

	/** DenseMap iterator implementation. */
	template<class Key, class Value, class KeyInfo, class ValueInfo>
	class DenseMapIterator
	{
	public:
		typedef std::pair<Key, Value> DensePair;

		DenseMapIterator(const DensePair* _ptr, const DensePair* _end)
			:ptr(_ptr), end(_end)
		{
			next();
		}

		std::pair<Key, Value>& operator* () const { return *const_cast<DensePair*>(ptr); }
		std::pair<Key, Value>* operator-> () const { return const_cast<DensePair*>(ptr); }

		bool operator== (const DenseMapIterator& rhs) const { return ptr == rhs.ptr; }
		bool operator!= (const DenseMapIterator& rhs) const { return ptr != rhs.ptr; }

		DenseMapIterator& operator++ ()
		{
			++ptr;
			next();
			return *this;
		}

		DenseMapIterator operator++ (int)
		{
			DenseMapIterator tmp = *this;
			++*this;
			return tmp;
		}

	protected:
		const DensePair* ptr;
		const DensePair* end;

	private:
		void next()
		{
			while (ptr != end &&
				(KeyInfo::equal(ptr->first, KeyInfo::getEmptyKey()) ||
					KeyInfo::equal(ptr->first, KeyInfo::getTombstoneKey())))
				++ptr;
		}
	};

	/** DenseMap constant iterator implementation. */
	template <class Key, class Value, class KeyInfo, class ValueInfo>
	class DenseMapConstIterator final : public DenseMapIterator<Key, Value, KeyInfo>
	{
	public:
		DenseMapConstIterator(const std::pair<Key, Value>* _ptr, const std::pair<Key, Value>* _end)
			:DenseMapIterator<Key, Value, KeyInfo>(_ptr, _end)
		{ }

		const std::pair<Key, Value>& operator* () const { return *this->ptr; }
		const std::pair<Key, Value>* operator-> () const { return this->ptr; }
	};

	/**
	* DenseMap data structure implementation.
	* It keeps all its data in one memory allocation, giving away with buckets
	* in favor of keeping keys and values next to each other in memory.
	* It allocates a large number of pairs by default is 64. With small sizes the insertions are nearly free
	*/
	template<class Key, class Value,
		class KeyInfo = DenseMapInfo<Key>,
		class ValueInfo = DenseMapInfo<Value>,
		UINT32 Size = 64>
		class DenseMap
	{
	public:
		typedef std::pair<Key, Value> DensePair;
		typedef DenseMapIterator<Key, Value, KeyInfo> Iterator;
		typedef DenseMapConstIterator<Key, Value, KeyInfo> ConstIterator;

		DenseMap() = default;
		DenseMap(const DenseMap<Key, Value>& other)
		{
			mCount = 0;
			if (!other.empty())
				copy(other);
		}

		DenseMap(DenseMap<Key, Value>&& other)
		{
			mCount = 0;
			if (!other.empty())
				copy(std::move(other));
		}

		DenseMap(UINT32 n)
		{
			append(n);
		}

		~DenseMap()
		{
			for (DensePair* pair = getBuckets(), *e = getBuckets() + getCount(); pair != e; ++pair)
			{
				if (!KeyInfo::equal(pair->first, getEmptyKey()) &&
					!KeyInfo::equal(pair->first, getTombstoneKey()))
					pair->second.~Value();

				pair->first.~Key();
			}

			bs_free(mBuckets);
		}

		Value& operator[] (const Key& key)
		{
			return construct(key).second;
		}

		DenseMap& operator= (const DenseMap<Key, Value>& other)
		{
			if (this == other)
				return *this;

			copy(other);
			return *this;
		}

		bool operator== (const DenseMap<Key, Value>& other) const
		{
			if (this->size() != other.size()) return false;
			return std::equal(this->begin(), this->end(), other.begin());
		}

		bool operator!= (const DenseMap<Key, Value>& other) const
		{
			return !(*this == other);
		}

		Iterator begin()
		{
			return Iterator(mBuckets, mBuckets + mCount);
		}

		Iterator end()
		{
			return Iterator(mBuckets + mCount, mBuckets + mCount);
		}

		ConstIterator begin() const
		{
			return ConstIterator(mBuckets, mBuckets + mCount);
		}

		ConstIterator end() const
		{
			return ConstIterator(mBuckets + mCount, mBuckets + mCount);
		}

		bool empty() const { return mEntries == 0; }
		UINT32 size() const { return mEntries; }

		UINT32 getCount() const { return mCount; }
		DensePair* getBuckets() const { return mBuckets; }
		UINT32 getTombstones() const { return mTombstones; }

		void init()
		{
			append(Size);
		}

		/** Grow so that it has at least n mBuckets. */
		void resize(UINT32 n)
		{
			grow(n);
		}

		void clear()
		{
			// If the capacity of the array is huge and the elements used is small then shrink the array.
			if (mEntries * 4 < mCount && mCount > Size)
			{
				shrink();
				return;
			}

			const Key emptyKey = getEmptyKey();
			for (DensePair* pair = getBuckets(), *e = getBuckets() + getCount(); pair != e; ++pair)
			{
				if (!KeyInfo::equal(pair->first, emptyKey))
				{
					if (!KeyInfo::equal(pair->first, getTombstoneKey()))
					{
						pair->second.~Value();
						--mEntries;
					}

					pair->first = emptyKey;
				}
			}

			assert(size() == 0 && "The number of nodes is imbalance");
			mTombstones = 0;
		}

		bool contains(const Key& key) const
		{
			DensePair* temp;
			return lookup(key, temp);
		}

		Iterator find(const Key& key)
		{
			DensePair* temp;

			if (lookup(key, temp))
				return Iterator(temp, mBuckets + mCount);

			return end();
		}

		ConstIterator find(const Key& key) const
		{
			DensePair* temp;

			if (lookup(key, temp))
				return ConstIterator(temp, mBuckets + mCount);

			return end();
		}

		std::pair<Iterator, bool> insert(const std::pair<Key, Value>& pair)
		{
			if (empty())
				init();

			DensePair* temp;

			// It is already in map
			if (lookup(pair.first, temp))
				return std::make_pair(Iterator(temp, getBuckets() + getCount()), false);

			// Otherwise insert the new element
			temp = insertBucket(pair.first, pair.second, temp);

			return std::make_pair(Iterator(temp, getBuckets() + getCount()), true);
		}

		bool erase(const Key& key)
		{
			assert(size() != 0);
			DensePair* temp;

			if (!lookup(key, temp))
				return false;

			update(temp);

			return true;
		}

		bool erase(Iterator iter)
		{
			assert(size() != 0);
			DensePair* temp = &*iter;

			update(temp);

			return true;
		}

		DensePair& construct(const Key& key)
		{
			DensePair* temp;

			if (lookup(key, temp))
				return temp;

			return insertBucket(key, Value(), temp);
		}

	private:
		void update(DensePair* t)
		{
			t->second.~Value();
			t->first = getTombstoneKey();
			--mEntries;
			++mTombstones;
		}

		void copy(const DenseMap<Key, Value>& other)
		{
			if (getCount() != 0 && (!KeyInfo::pod() || !ValueInfo::pod()))
			{
				const Key EmptyKey = getEmptyKey();
				const Key TombstoneKey = getTombstoneKey();
				for (DensePair* pair = getBuckets(), *e = getBuckets() + getCount(); pair != e; ++pair)
				{
					if (!KeyInfo::equal(pair->first, EmptyKey) && !KeyInfo::equal(pair->first, TombstoneKey))
						pair->second.~Value();

					pair->first.~Key();
				}
			}

			mEntries = other.size();
			mTombstones = other.getTombstones();

			if (getCount())
				bs_free(mBuckets);

			mBuckets = bs_allocN<DensePair>(other.getCount());

			if (KeyInfo::pod() && ValueInfo::pod())
				memcpy(getBuckets(), other.getBuckets(), other.getCount() * sizeof(DensePair));
			else
				for (UINT32 i = 0; i < other.getCount(); ++i)
				{
					new (&mBuckets[i].first) Key(other.mBuckets[i].first);

					if (!KeyInfo::equal(mBuckets[i].first, getEmptyKey()) &&
						!KeyInfo::equal(mBuckets[i].first, getTombstoneKey()))
						new (&mBuckets[i].second) Value(other.mBuckets[i].second);
				}

			mCount = other.getCount();
		}

		DensePair* insertBucket(const Key& key, const Value& value, DensePair* temp)
		{
			// If the hash table load is more than 3/4 or is it fewer than 1/8 of the buckets then grow the table.
			// If there is one empty bucket with tons of tombstones, missing lookups would have to
			// probe almost the entire table until it found the empty bucket. If the
			// table is completely filled with tombstones, no lookup would ever succeed, making init loops in lookup.
			if (size() * 4 >= getCount() * 3 || getCount() - (size() + getTombstones()) < getCount() / 8)
			{
				this->grow(getCount() * 2);
				lookup(key, temp);
			}

			++mEntries;

			// If we are writing over a tombstone, remember this
			if (!KeyInfo::equal(temp->first, getEmptyKey()))
				--mTombstones;

			temp->first = key;
			new (&temp->second) Value(value);

			return temp;
		}

		static UINT32 getHashValue(const Key& key) { return KeyInfo::getHashValue(key); }
		static const Key getEmptyKey() { return KeyInfo::getEmptyKey(); }
		static const Key getTombstoneKey() { return KeyInfo::getTombstoneKey(); }

		/**
		* Lookup the appropriate bucket for key, returning it in @p f.
		* the bucket contains the key and a value, this returns true, otherwise
		* it returns a bucket with an empty marker or tombstone and returns false.
		*/
		bool lookup(const Key& key, DensePair*& f) const
		{
			UINT32 bucketN = getHashValue(key);
			UINT32 probe = 1;
			DensePair* bucket = getBuckets();

			// Keep track of whether we find a tombstone while probing.
			DensePair* foundTombstone = 0;

			const Key EmptyKey = getEmptyKey();
			const Key TombstoneKey = getTombstoneKey();
			assert(!KeyInfo::equal(key, EmptyKey) &&
				!KeyInfo::equal(key, TombstoneKey) &&
				"Empty/Tombstone value shouldn't be inserted into map!");

			while (1)
			{
				DensePair* temp = bucket + (bucketN & (getCount() - 1));

				// If found the key bucket return it.
				if (KeyInfo::equal(temp->first, key))
				{
					f = temp;
					return true;
				}

				// If we found an empty bucket, the key doesn't exist in the set.
				// Insert it and return the default value.
				if (KeyInfo::equal(temp->first, EmptyKey))
				{
					// If we've already seen a tombstone while probing, fill it in instead
					// of the empty bucket we eventually probed to.
					if (foundTombstone)
						temp = foundTombstone;

					f = foundTombstone ? foundTombstone : temp;

					return false;

				}

				// If this is a tombstone, remember it.  If key ends up not in the map, we
				// prefer to return it than something that would require more probing.
				if (KeyInfo::equal(temp->first, TombstoneKey) && !foundTombstone)
					foundTombstone = temp;

				// Otherwise it's a hash collision or a tombstone, continue quadratic probing.
				bucketN += probe++;
			}
		}

		void append(UINT32 n)
		{
			mEntries = 0;
			mTombstones = 0;
			mCount = n;
			assert(n && (n & (n - 1)) == 0 && "initial buckets must be a power of two!");

			mBuckets = bs_allocN<DensePair>(n);

			for (UINT32 i = 0; i != n; ++i)
				new (&mBuckets[i].first) Key(getEmptyKey());
		}

		void grow(UINT32 n)
		{
			const UINT32 oldCount = getCount();
			DensePair* oldBuckets = getBuckets();

			// Double the number of buckets
			while (mCount <= n)
				mCount <<= 1;

			mTombstones = 0;
			mBuckets = bs_allocN<DensePair>(getCount());

			// Initialize all the keys to EmptyKey
			const Key EmptyKey = getEmptyKey();
			for (UINT32 i = 0, e = mCount; i != e; ++i)
				new (&mBuckets[i].first) Key(EmptyKey);

			// Insert all the old elements
			const Key TombstoneKey = getTombstoneKey();
			for (DensePair* pair = oldBuckets, *e = oldBuckets + oldCount; pair != e; ++pair)
			{
				if (!KeyInfo::equal(pair->first, EmptyKey) && !KeyInfo::equal(pair->first, TombstoneKey))
				{
					// Insert the pair into the new table
					DensePair* destBucket;
					bool f = lookup(pair->first, destBucket);

					// silence warning
					f = f;
					assert(!f && "Key already in new map");

					destBucket->first = pair->first;
					new (&destBucket->second) Value(pair->second);

					// Free the value
					pair->second.~Value();
				}

				pair->first.~Key();
			}
			// Free the old table
			bs_free(oldBuckets);
		}

		void shrink()
		{
			const UINT32 oldCount = getCount();
			DensePair* oldBuckets = getBuckets();

			// Reduce the number of mBuckets
			mCount = size() > 32 ? 1 << ((int)std::log(size()) + 1) : 64;
			mTombstones = 0;
			mBuckets = bs_allocN<DensePair>(getCount());

			// Initialize the keys to EmptyKey
			const Key EmptyKey = getEmptyKey();
			for (UINT32 i = 0, e = mCount; i != e; ++i)
				new (&mBuckets[i].first) Key(EmptyKey);

			// Free the old mBuckets
			const Key TombstoneKey = getTombstoneKey();
			for (DensePair* B = oldBuckets, *E = oldBuckets + oldCount; B != E; ++B)
			{
				if (!KeyInfo::equal(B->first, EmptyKey) && !KeyInfo::equal(B->first, TombstoneKey))
					B->second.~Value();

				B->first.~Key();
			}

			// Free the old mBuckets
			bs_free(oldBuckets);
			mEntries = 0;
		}

	private:
		DensePair* mBuckets = nullptr;
		UINT32 mCount = 0;
		UINT32 mEntries = 0;
		UINT32 mTombstones = 0;
	};

	/** @} */
}
