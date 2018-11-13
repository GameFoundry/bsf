#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	template <class Type>
	class DynArray final
	{
	public:
		typedef Type ValueType;
		typedef Type* Iterator;
		typedef const Type* ConstIterator;
		typedef std::reverse_iterator<Type*> ReverseIterator;
		typedef std::reverse_iterator<const Type*> ConstReverseIterator;
		typedef ptrdiff_t DifferenceType;

		DynArray() = default;

		explicit DynArray(UINT32 n)
		{
			reallocate(n, false);
		}

		DynArray(UINT32 n, const ValueType& value)
		{
			append(n, value);
		}

		DynArray(Iterator first, Iterator last)
		{
			append(first, last);
		}

		DynArray(std::initializer_list<ValueType> list)
		{
			append(list);
		}

		DynArray(const DynArray<ValueType>& other)
		{
			if (!other.empty())
				*this = other;
		}

		DynArray(DynArray<ValueType>&& other)
		{
			if (!other.empty())
				*this = std::move(other);
		}

		~DynArray()
		{
			for (auto& entry : *this)
				entry.~Type();

			bs_free(mArr);
		}

		DynArray<ValueType>& operator= (const DynArray<ValueType>& other)
		{
			if (this == &other)
				return *this;

			if (mCapacity < other.mSize)
				reallocate(other.mSize, true);

			for (UINT32 i = 0; i < other.mSize; ++i)
				mArr[i] = other.mArr[i];

			mSize = other.mSize;

			return *this;
		}

		DynArray<ValueType>& operator= (DynArray<ValueType>&& other)
		{
			if (mCapacity < other.mSize)
				reallocate(other.mSize, true);

			for (UINT32 i = 0; i < other.mSize; ++i)
				mArr[i] = std::move(other.mArr[i]);

			mSize = other.mSize;

			return *this;
		}

		DynArray<ValueType>& operator= (std::initializer_list<ValueType> list)
		{
			if (capacity() < list.size())
				reallocate(list.size(), true);

			mSize = 0;

			for (auto& i : list)
				mArr[mSize++] = i;

			return *this;
		}

		bool operator== (const DynArray<ValueType>& other) const
		{
			if (this->size() != other.size()) return false;
			return std::equal(this->begin(), this->end(), other.begin());
		}

		bool operator!= (const DynArray<ValueType>& other) const
		{
			return !(*this == other);
		}

		bool operator< (const DynArray<ValueType>& other) const
		{
			return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
		}

		bool operator> (const DynArray<ValueType>& other) const
		{
			return other < *this;
		}

		bool operator<= (const DynArray<ValueType>& other) const
		{
			return !(other < *this);
		}

		bool operator>= (const DynArray<ValueType>& other) const
		{
			return !(*this < other);
		}

		Type& operator[] (UINT32 index)
		{
			assert(index < mSize && "Array index out-of-range.");

			return mArr[index];
		}

		const Type& operator[] (UINT32 index) const
		{
			assert(index < mSize && "Array index out-of-range.");

			return mArr[index];
		}

		bool empty() const { return mSize == 0; }

		Iterator begin() { return mArr; }
		Iterator end() { return mArr + mSize; }

		ConstIterator begin() const { return mArr; }
		ConstIterator end() const { return mArr + mSize; }

		ConstIterator cbegin() const { return mArr; }
		ConstIterator cend() const { return mArr + mSize; }

		ReverseIterator rbegin() { return ReverseIterator(end()); }
		ReverseIterator rend() { return ReverseIterator(begin()); }

		ConstReverseIterator rbegin() const { return ReverseIterator(end()); }
		ConstReverseIterator rend() const { return ReverseIterator(begin()); }

		ConstReverseIterator crbegin() const { return ReverseIterator(end()); }
		ConstReverseIterator crend() const { return ReverseIterator(begin()); }

		UINT32 size() const { return mSize; }
		UINT32 capacity() const { return mCapacity; }

		Type* data() { return mArr; }
		const Type* data() const { return mArr; }

		Type& at(unsigned index)
		{
			assert(index < mSize && "Array index out-of-range.");
			return (*this)[index];
		}

		const Type& at(unsigned index) const
		{
			assert(index < mSize && "Array index out-of-range.");
			return (*this)[index];
		}

		Type& front()
		{
			assert(!empty());
			return *mArr[0];
		}

		Type& back()
		{
			assert(!empty());
			return *mArr[mSize - 1];
		}

		const Type& front() const
		{
			assert(!empty());
			return mArr[0];
		}

		const Type& back() const
		{
			assert(!empty());
			return mArr[mSize - 1];
		}

		void add(const Type& element)
		{
			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(element);
		}

		void add(Type&& element)
		{
			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(std::move(element));
		}

		void pop()
		{
			assert(mSize > 0 && "Popping an empty array.");
			--mSize;
			mArr[mSize].~Type();
		}

		void remove(UINT32 index)
		{
			erase(begin() + index);
		}

		bool contains(const Type& element)
		{
			for (UINT32 i = 0; i < mSize; i++)
			{
				if (mArr[i] == element)
					return true;
			}

			return false;
		}

		void removeValue(const Type& element)
		{
			for (UINT32 i = 0; i < mSize; i++)
			{
				if (mArr[i] == element)
				{
					remove(i);
					break;
				}
			}
		}

		void clear()
		{
			for (UINT32 i = 0; i < mSize; ++i)
				mArr[i].~Type();

			mSize = 0;
		}

		void resize(UINT32 size, const Type& value = Type())
		{
			if (size > capacity())
				reallocate(size, true);

			if (size > mSize)
			{
				for (UINT32 i = mSize; i < size; i++)
					new (&mArr[i]) Type(value);
			}
			else
			{
				for (UINT32 i = size; i < mSize; i++)
					mArr[i].~Type();
			}

			mSize = size;
		}

		void reserve(UINT32 size)
		{
			if (size > mCapacity)
				reallocate(size, false);
		}

		void shrink()
		{
			reallocate(mSize, true);
		}

		void append(ConstIterator start, ConstIterator end)
		{
			const UINT32 count = (UINT32)std::distance(start, end);

			if (count > capacity())
				reallocate(count, false);

			std::uninitialized_copy(start, end, this->end());
			mSize += count;
		}

		void append(UINT32 count, const Type& element)
		{
			if ((size() + count) > capacity())
				reallocate(count, false);

			std::uninitialized_fill_n(end(), count, element);
			mSize += count;
		}

		void append(std::initializer_list<Type> list)
		{
			append(list.begin(), list.end());
		}

		void swap(DynArray<ValueType>& other)
		{
			const UINT32 tmpSize = size();
			const UINT32 tmpCapacity = capacity();
			Type* tmp = data();

			mSize = other.size();
			mCapacity = other.capacity();
			mArr = other.data();

			other.mSize = tmpSize;
			other.mCapacity = tmpCapacity;
			other.mArr = tmp;
		}

		template <typename ...Args>
		void emplaceBack(Args&& ...args)
		{
			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(std::forward<Args>(args) ...);
		}

		template <typename ...Args>
		Iterator emplace(ConstIterator it, Args&&... args)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();

			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(std::forward<Args>(args) ...);
			std::rotate(begin() + offset, end() - 1, end());

			return begin() + offset;
		}

		Iterator insert(ConstIterator it, const ValueType& element)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();

			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(element);
			std::rotate(begin() + offset, end() - 1, end());

			return begin() + offset;
		}

		Iterator insert(ConstIterator it, ValueType&& element)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();

			if (size() == capacity())
			{
				if (capacity() == 0)
					reallocate(1, false);
				else
					reallocate(mCapacity * 2, true);
			}

			new (&mArr[mSize++]) Type(std::move(element));
			std::rotate(begin() + offset, end() - 1, end());

			return begin() + offset;
		}

		Iterator insert(ConstIterator it, UINT32 n, const ValueType& element)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();
			Iterator iter = &mArr[offset];

			if (!n)
				return iter;

			if (size() + n > capacity())
				reallocate((mSize + n) * 2, true);

			UINT32 c = n;
			while (c--)
				new (&mArr[mSize++]) Type(element);

			std::rotate(begin() + offset, end() - n, end());

			return begin() + offset;
		}

		template <typename InputIt>
		typename std::enable_if<!std::is_integral<InputIt>::value, void>::type insert(ConstIterator it,
			InputIt first, InputIt last)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();
			DifferenceType n = last - first;

			if (size() + n > capacity())
				reallocate((mSize + n) * 2, true);

			while (first != last)
				new (&mArr[mSize++]) Type(*first++);

			std::rotate(begin() + offset, end() - n, end());
		}

		Iterator insert(ConstIterator it, std::initializer_list<ValueType> list)
		{
			Iterator iterc = const_cast<Iterator>(it);
			DifferenceType offset = iterc - begin();
			Iterator iter = &mArr[offset];
			UINT32 n = list.size();

			if (!n)
				return iter;

			if (size() + n > capacity())
				reallocate((mSize + n) * 2, true);

			for (auto& entry : list)
				new (&mArr[mSize++]) Type(entry);

			std::rotate(begin() + offset, end() - n, end());

			return iter;
		}

		Iterator erase(ConstIterator first, ConstIterator last)
		{
			assert(first >= begin() && "Iterator to insert is out of bounds.");
			assert(last < end() && "Inserting at past-the-end iterator.");

			Iterator iter = &mArr[first - begin()];

			if (first == last)
				return iter;

			Iterator iterFirst = const_cast<Iterator>(first);
			Iterator iterLast = const_cast<Iterator>(last);

			for (; iterFirst != iterLast; ++iterFirst)
				(*iterFirst).~Type();

			std::move(iterLast, end(), iter);

			for (Iterator it = iter; it < iterLast; ++it)
				pop();

			mSize -= iterLast - iterFirst;

			return iter;
		}

		Iterator erase(ConstIterator it)
		{
			assert(it >= begin() && "Iterator to erase is out of bounds.");
			assert(it < end() && "Erasing at past-the-end iterator.");

			Iterator iter = &mArr[it - begin()];

			Iterator toErase = const_cast<Iterator>(it);
			std::move(toErase + 1, end(), toErase);
			pop();

			return iter;
		}

	private:
		void reallocate(UINT32 capacity, bool data)
		{
			Type* tmp = bs_allocN<Type>(capacity);

			if (mArr)
			{
				UINT32 oldSize = mSize;
				if (data)
				{
					if (mSize > capacity)
						mSize = capacity;

					for (UINT32 i = 0; i < mSize; i++)
						tmp[i] = mArr[i];
				}
				else
					mSize = 0;

				for (UINT32 i = 0; i < oldSize; i++)
					mArr[i].~Type();
			}

			mArr = tmp;
			mCapacity = capacity;
		}

		Type* mArr = nullptr;
		UINT32 mSize = 0;
		UINT32 mCapacity = 0;
	};
}
