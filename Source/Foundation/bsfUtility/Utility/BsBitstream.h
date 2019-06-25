
//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Math/BsMath.h"
#include "Math/BsQuaternion.h"
#include "Math/BsVector3.h"
#include "Utility/BsBitwise.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	/**
	 * Allows encoding/decoding of types into a stream of bits. Supports various methods of storing data in a compact form.
	 * The bitstream can manage its internal memory or a user can provide an external source of data. If using internal
	 * memory the bitstream will automatically grow the memory storage as needed.
	 *
	 * The stream keeps an internal cursor that represents the bit at which to perform read & write operations.
	 * Read & write operations will operate at the current cursor location and the cursor will be advanced by the number of
	 * bits read or written. If writing outside of range the internal memory buffer will be automatically expanded, except
	 * when external memory buffer is used, in which case it is undefined behaviour. Reading outside of range is always
	 * undefined behaviour.
	 */
	class Bitstream
	{
		using QuantType = uint8_t;
	public:
		/**
		 * Initializes an empty bitstream. As data is written the stream will grow its internal memory storage
		 * automatically.
		 */
		Bitstream() = default;

		/**
		 * Initializes a bitstream with some initial capacity. If more bytes than capacity is written, the bitstream will
		 * grow its internal memory storage.
		 *
		 * @param[in]	capacity	Number of bytes to initially allocate for the internal memory storage.
		 */
		Bitstream(uint32_t capacity);

		/**
		 * Initializes a bitstream with external data storage. The bitstream will not manage internal memory and will not
		 * grow memory storage if capacity is exceeded. The user is responsible for keeping track and not writing outside
		 * of buffer range.
		 *
		 * @param[in]	data	Address of the external memory buffer. The user is responsible of keeping this memory alive
		 *						for the lifetime of the bitstream, as well as releasing it. Must have enough capacity to
		 *						store @p count bits.
		 * @param[in]	count	Size of the provided data, in bits.
		 */
		Bitstream(QuantType* data, uint32_t count);

		~Bitstream();

		/**
		 * Writes bits from the provided buffer into the stream at the current cursor location and advances the cursor.
		 *
		 * @param[in]	data	Buffer to write the data from. Must have enough capacity to store @p count bits.
		 * @param[in]	count	Number of bits to write.
		 */
		void writeBits(const QuantType* data, uint32_t count);

		/**
		 * Reads bits from the stream into the provided buffer from the current cursor location and advances the cursor.
		 *
		 * @param[out]	data	Buffer to read the data from. Must have enough capacity to store @p count bits.
		 * @param[in]	count	Number of bits to read.
		 */
		void readBits(QuantType* data, uint32_t count);

		/**
		 * Writes the provided data into the stream at the current cursor location and advances the cursor.
		 *
		 * @param[in]	value	Data to write.
		 */
		template<class T>
		void write(const T& value);

		/**
		 * Reads bits from the stream and writes them into the provided object. Data is read from the current cursor
		 * location and advances the cursor.
		 *
		 * @param[out]	value	Object to initialize with the read bits.
		 */
		template<class T>
		void read(T& value);

		/** @copydoc write(const T&) */
		void write(const bool& value);

		/** @copydoc read(T&) */
		void read(bool& value);

		/** @copydoc write(const T&) */
		void write(const String& value);

		/** @copydoc read(T&) */
		void read(String& value);

		/**
		 * Checks if the provided value differs from the last provided value, and if they are equivalent writes just a
		 * single bit signifying no change. Otherwise the value is encoded as if calling write().
		 */
		template<class T>
		void writeDelta(const T& value, const T& lastValue);

		/** Reads the data written by writeDelta() from the current cursor location and advances the cursor. */
		template<class T>
		void readDelta(T& value, const T& lastValue);

		/** @copydoc writeDelta(const T&, const T&) */
		void writeDelta(bool value, bool lastValue);

		/** @copydoc readDelta(T&, const T&) */
		void readDelta(bool& value, bool lastValue);

		/**
		 * Encodes a 32-bit integer value as a base-128 varint and writes it to the stream. Write is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void writeVarInt(uint32_t value);

		/**
		 * Encodes a 32-bit integer value as a base-128 varint and writes it to the stream. Write is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void writeVarInt(int32_t value);

		/**
		 * Encodes a 64-bit integer value as a base-128 varint and writes it to the stream. Write is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void writeVarInt(uint64_t value);

		/**
		 * Encodes a 64-bit integer value as a base-128 varint and writes it to the stream. Write is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void writeVarInt(int64_t value);

		/**
		 * Decodes a 32-bit integer value encoded as a base-128 varint from the stream. Read is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void readVarInt(uint32_t& value);

		/**
		 * Decodes a 32-bit integer value encoded as a base-128 varint from the stream. Read is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void readVarInt(int32_t& value);

		/**
		 * Decodes a 32-bit integer value encoded as a base-128 varint from the stream. Read is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void readVarInt(uint64_t& value);

		/**
		 * Decodes a 32-bit integer value encoded as a base-128 varint from the stream. Read is performed at the
		 * current cursor location and advances the cursor. Varints are a method of serializing integers using one or
		 * more bytes, where smaller values use less bytes.
		 */
		void readVarInt(int64_t& value);

		/**
		 * Checks if the provided value differs from the last provided value, and if they are equivalent writes just a
		 * single bit signifying no change. Otherwise the value is encoded as if calling writeVarInt().
		 */
		template<class T>
		void writeVarIntDelta(const T& value, const T& lastValue);

		/** Reads the data written by writeVarIntDelta() from the current cursor location and advances the cursor. */
		template<class T>
		void readVarIntDelta(T& value, const T& lastValue);

		/**
		 * Encodes a float in range [0, 1] into a fixed point representation using a specific number of bits, and writes it
		 * to the stream. Write is performed at the current cursor location and advances the cursor.
		 */
		void writeNorm(float value, uint32_t bits = 16);

		/**
		 * Decodes a float encoded using writeNorm(float, uint32_t). Read is performed at the current cursor location and
		 * advances the cursor. Same number of bits need to be used as when the float was encoded.
		 */
		void readNorm(float& value, uint32_t bits = 16);

		/**
		 * Encodes a 3D vector with individual components in range [-1, 1] into a fixed point representation where each
		 * component uses a specific number of bits, and writes it to the stream. Write is performed at the current cursor
		 * location and advances the cursor.
		 */
		void writeNorm(const Vector3& value, uint32_t bits = 16);

		/**
		 * Decodes a 3D vector encoded using writeNorm(Vector3, uint32_t). Read is performed at the current cursor location
		 * and advances the cursor. Same number of bits need to be used as when the float was encoded.
		 */
		void readNorm(Vector3& value, uint32_t bits = 16);

		/**
		 * Encodes a quaternion with individual components in range [-1, 1] into a fixed point representation where each
		 * component uses a specific number of bits, and writes it to the stream. Write is performed at the current cursor
		 * location and advances the cursor.
		 */
		void writeNorm(const Quaternion& value, uint32_t bits = 16);

		/**
		 * Decodes a quaternion encoded using writeNorm(Quaternion, uint32_t). Read is performed at the current cursor
		 * location and advances the cursor. Same number of bits need to be used as when the float was encoded.
		 */
		void readNorm(Quaternion& value, uint32_t bits = 16);

		/**
		 * Checks if the provided value differs from the last provided value, and if they are equivalent writes just a
		 * single bit signifying no change. Otherwise the value is encoded as if calling writeNorm().
		 */
		template<class T>
		void writeNormDelta(const T& value, const T& lastValue, uint32_t bits = 16);

		/** Reads the data written by writeNormDelta() from the current cursor location and advances the cursor. */
		template<class T>
		void readNormDelta(T& value, const T& lastValue, uint32_t bits = 16);

		/**
		 * Encodes an integer in a specific range, using the range the reduce the number of bits required, and writes it
		 * to the stream. Write is performed at the current cursor location and advances the cursor.
		 */
		template<class T>
		void writeRange(const T& value, const T& min, const T& max);

		/**
		 * Decodes an integer encoded using writeRange(const T&, const T&, const T&). Read is performed at the current
		 * cursor location and advances the cursor. Same needs to be used as when the value was encoded.
		 */
		template<class T>
		void readRange(T& value, const T& min, const T& max);

		/**
		 * Checks if the provided value differs from the last provided value, and if they are equivalent writes just a
		 * single bit signifying no change. Otherwise the value is encoded as if calling
		 * writeRange(const T&, const T&, const T&).
		 */
		template<class T>
		void writeRangeDelta(const T& value, const T& lastValue, const T& min, const T& max);

		/**
		 * Reads the data written by writeRangeDelta(const T&, const T&, const T&, const T&) from the current cursor
		 * location and advances the cursor.
		 */
		template<class T>
		void readRangeDelta(T& value, const T& lastValue, const T& min, const T& max);

		/**
		 * Encodes a float in a specific range into a fixed point representation using a specific number of bits, and
		 * writes it to the stream. Write is performed at the current cursor location and advances the cursor.
		 */
		void writeRange(float value, float min, float max, uint32_t bits = 16);

		/**
		 * Decodes a float encoded using writeRange(float, float, float, uint32_t). Read is performed at the current cursor
		 * location and advances the cursor. Same number of bits, and the same range needs to be used as when the float was
		 * encoded.
		 */
		void readRange(float& value, float min, float max, uint32_t bits = 16);

		/**
		 * Checks if the provided value differs from the last provided value, and if they are equivalent writes just a
		 * single bit signifying no change. Otherwise the value is encoded as if calling
		 * writeRange(float, float, float, uint32_t).
		 */
		void writeRangeDelta(float value, float lastValue, float min, float max, uint32_t bits = 16);

		/**
		 * Reads the data written by writeRangeDelta(float, float, float, float, uint32_t) from the current cursor
		 * location and advances the cursor.
		 */
		void readRangeDelta(float& value, float lastValue, float min, float max, uint32_t bits = 16);

		/**
		 * Skip a defined number of bits, moving the read/write cursor by this amount. This can also be a negative value,
		 * in which case the file pointer rewinds a defined number of bits. Note the cursor can never skip past the
		 * capacity of the buffer, and will be clamped.
		 */
		void skip(int32_t count);

		/**
		 * Repositions the read/write cursor to the specified bit. Note the cursor can never skip past the capacity
		 * of the buffer, and will be clamped.
		 */
		void seek(uint32_t pos);

		/**
		 * Aligns the read/write cursor to a byte boundary. @p count determines the alignment in bytes. Note the
		 * requested alignment might not be achieved if count > 1 and it would move the cursor past the capacity of the
		 * buffer, as the cursor will be clamped to buffer end regardless of alignment.
		 */
		void align(uint32_t count = 1);

		/** Returns the current read/write cursor position, in bits. */
		uint32_t tell() const { return mCursor; }

		/** Returns true if the stream has reached the end. */
		bool eof() const { return mCursor >= mNumBits; }

		/** Returns the total number of bits available in the stream. */
		uint32_t size() const { return mNumBits; }

		/** Returns the total number of bits the stream can store without needing to allocate more memory. */
		uint32_t capacity() const { return mMaxBits; }

		/** Returns the internal data buffer. */
		QuantType* data() const { return mData; }

	private:
		// c++17 feature allowing for inline variables, otherwise get undefined reference warning.
		inline static constexpr uint32_t BYTES_PER_QUANT = sizeof(QuantType);
		inline static constexpr uint32_t BITS_PER_QUANT = BYTES_PER_QUANT * 8;
		inline static constexpr uint32_t BITS_PER_QUANT_LOG2 = Bitwise::bitsLog2(BITS_PER_QUANT);

		/** Checks if the internal memory buffer needs to grow in order to accomodate @p numBits bits. */
		void reallocIfNeeded(uint32_t numBits);

		/** Reallocates the internal buffer making enough room for @p numBits (rounded to a multiple of BYTES_PER_QUANT. */
		void realloc(uint32_t numBits);

		QuantType* mData = nullptr;
		uint32_t mMaxBits = 0;
		uint32_t mNumBits = 0;
		bool mOwnsMemory = true;

		uint32_t mCursor = 0;
	};

	/** @} */

	/** @addtogroup Implementation
	 *  @{
	 */

	inline Bitstream::Bitstream(uint32_t capacity)
	{
		realloc(capacity * 8);
	}

	inline Bitstream::Bitstream(QuantType* data, uint32_t count)
		: mData(data), mMaxBits(count), mNumBits(count), mOwnsMemory(false) { }

	inline Bitstream::~Bitstream()
	{
		if (mData && mOwnsMemory)
			bs_free(mData);
	}

	inline void Bitstream::writeBits(const QuantType* data, uint32_t count)
	{
		if (count == 0)
			return;

		uint32_t newCursor = mCursor + count;
		reallocIfNeeded(newCursor);

		uint32_t destBitsMod = mCursor & (BITS_PER_QUANT - 1);
		uint32_t destQuant = mCursor >> BITS_PER_QUANT_LOG2;
		uint32_t destMask = (1 << destBitsMod) - 1;

		// If destination is aligned, memcpy everything except the last quant (unless it is also aligned)
		if (destBitsMod == 0)
		{
			uint32_t numQuants = count >> BITS_PER_QUANT_LOG2;
			memcpy(&mData[destQuant], data, numQuants * BYTES_PER_QUANT);

			data += numQuants;
			count -= numQuants * BITS_PER_QUANT;
			destQuant += numQuants;
		}

		// Write remaining bits (or all bits if destination wasn't aligned)
		while (count > 0)
		{
			QuantType quant = *data;
			data++;

			mData[destQuant] = (quant << destBitsMod) | (mData[destQuant] & destMask);

			uint32_t writtenBits = BITS_PER_QUANT - destBitsMod;
			if (count > writtenBits)
				mData[destQuant + 1] = (quant >> writtenBits) | (mData[destQuant + 1] & ~destMask);

			destQuant++;
			count -= std::min(BITS_PER_QUANT, count);
		}

		mCursor = newCursor;
		mNumBits = std::max(mNumBits, newCursor);
	}

	inline void Bitstream::readBits(QuantType* data, uint32_t count)
	{
		if (count == 0)
			return;

		assert((mCursor + count) <= mNumBits);

		uint32_t newCursor = mCursor + count;
		uint32_t srcBitsMod = mCursor & (BITS_PER_QUANT - 1);
		uint32_t srcQuant = mCursor >> BITS_PER_QUANT_LOG2;

		// If source is aligned, memcpy everything except the last quant (unless it is also aligned)
		if (srcBitsMod == 0)
		{
			uint32_t numQuants = count >> BITS_PER_QUANT_LOG2;
			memcpy(data, &mData[srcQuant], numQuants * BYTES_PER_QUANT);

			data += numQuants;
			count -= numQuants * BITS_PER_QUANT;
			srcQuant += numQuants;
		}

		// Read remaining bits (or all bits if source wasn't aligned)
		while (count > 0)
		{
			QuantType& quant = *data;
			data++;

			quant = 0;
			quant |= mData[srcQuant] >> srcBitsMod;

			uint32_t readBits = BITS_PER_QUANT - srcBitsMod;
			if (count > readBits)
				quant |= mData[srcQuant + 1] << readBits;

			srcQuant++;
			count -= std::min(BITS_PER_QUANT, count);
		}

		mCursor = newCursor;
	}

	template <class T>
	void Bitstream::write(const T& value)
	{
		writeBits((QuantType*)&value, sizeof(value) * 8);
	}

	template <class T>
	void Bitstream::read(T& value)
	{
		QuantType* temp = (QuantType*)&value;
		readBits(temp, sizeof(value) * 8);
	}

	inline void Bitstream::write(const bool& value)
	{
		reallocIfNeeded(mCursor + 1);

		uint32_t destBitsMod = mCursor & (BITS_PER_QUANT - 1);
		uint32_t destQuant = mCursor >> BITS_PER_QUANT_LOG2;

		if (value)
			mData[destQuant] |= 1U << destBitsMod;
		else
			mData[destQuant] &= ~(1U << destBitsMod);

		mCursor++;
		mNumBits = std::max(mNumBits, mCursor);
	}

	inline void Bitstream::read(bool& value)
	{
		assert((mCursor + 1) <= mNumBits);

		uint32_t srcBitsMod = mCursor & (BITS_PER_QUANT - 1);
		uint32_t srcQuant = mCursor >> BITS_PER_QUANT_LOG2;

		value = (mData[srcQuant] >> srcBitsMod) & 0x1;
		mCursor++;
	}

	inline void Bitstream::write(const String& value)
	{
		uint32_t length = (uint32_t)value.size();
		writeVarInt(length);
		writeBits((QuantType*)value.data(), length * 8);
	}

	inline void Bitstream::read(String& value)
	{
		uint32_t length;
		readVarInt(length);

		value.resize(length);

		QuantType* temp = (QuantType*)value.data();
		readBits(temp, length * 8);
	}

	template <class T>
	void Bitstream::writeDelta(const T& value, const T& lastValue)
	{
		if(value == lastValue)
			write(true);
		else
		{
			write(false);
			write(value);
		}
	}

	template <class T>
	void Bitstream::readDelta(T& value, const T& lastValue)
	{
		bool clean;
		read(clean);

		if(clean)
			value = lastValue;
		else
			read(value);
	}

	inline void Bitstream::writeDelta(bool value, bool lastValue)
	{
		write(value);
	}

	inline void Bitstream::readDelta(bool& value, bool lastValue)
	{
		read(value);
	}

	inline void Bitstream::writeVarInt(uint32_t value)
	{
		uint8_t output[5];
		uint32_t count = Bitwise::encodeVarInt(value, output);

		writeBits(output, count * 8);
	}

	inline void Bitstream::writeVarInt(int32_t value)
	{
		uint8_t output[5];
		uint32_t count = Bitwise::encodeVarInt(value, output);

		writeBits(output, count * 8);
	}

	inline void Bitstream::writeVarInt(uint64_t value)
	{
		uint8_t output[10];
		uint32_t count = Bitwise::encodeVarInt(value, output);

		writeBits(output, count * 8);
	}

	inline void Bitstream::writeVarInt(int64_t value)
	{
		uint8_t output[10];
		uint32_t count = Bitwise::encodeVarInt(value, output);

		writeBits(output, count * 8);
	}

	inline void Bitstream::readVarInt(uint32_t& value)
	{
		uint8_t output[5];
		for(uint32_t i = 0; i < 5; i++)
		{
			readBits(&output[i], 8);
			if((output[i] & 0x80) == 0)
				break;
		}

		Bitwise::decodeVarInt(value, output, 5);
	}

	inline void Bitstream::readVarInt(int32_t& value)
	{
		uint8_t output[5];
		for(uint32_t i = 0; i < 5; i++)
		{
			readBits(&output[i], 8);
			if((output[i] & 0x80) == 0)
				break;
		}

		Bitwise::decodeVarInt(value, output, 5);
	}

	inline void Bitstream::readVarInt(uint64_t& value)
	{
		uint8_t output[10];
		for(uint32_t i = 0; i < 10; i++)
		{
			readBits(&output[i], 8);
			if((output[i] & 0x80) == 0)
				break;
		}

		Bitwise::decodeVarInt(value, output, 10);
	}

	inline void Bitstream::readVarInt(int64_t& value)
	{
		uint8_t output[10];
		for(uint32_t i = 0; i < 10; i++)
		{
			readBits(&output[i], 8);
			if((output[i] & 0x80) == 0)
				break;
		}

		Bitwise::decodeVarInt(value, output, 10);
	}

	template <class T>
	void Bitstream::writeVarIntDelta(const T& value, const T& lastValue)
	{
		if(value == lastValue)
			write(true);
		else
		{
			write(false);
			writeVarInt(value);
		}
	}

	template <class T>
	void Bitstream::readVarIntDelta(T& value, const T& lastValue)
	{
		bool clean;
		read(clean);

		if(clean)
			value = lastValue;
		else
			readVarInt(value);
	}

	inline void Bitstream::writeNorm(float value, uint32_t bits)
	{
		uint32_t encodedVal = Bitwise::unormToUint(value, bits);
		writeBits((QuantType*)&encodedVal, bits);
	}

	inline void Bitstream::readNorm(float& value, uint32_t bits)
	{
		uint32_t encodedVal = 0;
		readBits((QuantType*)&encodedVal, bits);
		value = Bitwise::uintToUnorm(encodedVal, bits);
	}

	inline void Bitstream::writeNorm(const Vector3& value, uint32_t bits)
	{
		writeRange(value.x, -1.0f, 1.0f, bits);
		writeRange(value.y, -1.0f, 1.0f, bits);
		writeRange(value.z, -1.0f, 1.0f, bits);
	}

	inline void Bitstream::readNorm(Vector3& value, uint32_t bits)
	{
		readRange(value.x, -1.0f, 1.0f, bits);
		readRange(value.y, -1.0f, 1.0f, bits);
		readRange(value.z, -1.0f, 1.0f, bits);
	}

	inline void Bitstream::writeNorm(const Quaternion& value, uint32_t bits)
	{
		writeRange(value.x, -1.0f, 1.0f, bits);
		writeRange(value.y, -1.0f, 1.0f, bits);
		writeRange(value.z, -1.0f, 1.0f, bits);
		writeRange(value.w, -1.0f, 1.0f, bits);
	}

	inline void Bitstream::readNorm(Quaternion& value, uint32_t bits)
	{
		readRange(value.x, -1.0f, 1.0f, bits);
		readRange(value.y, -1.0f, 1.0f, bits);
		readRange(value.z, -1.0f, 1.0f, bits);
		readRange(value.w, -1.0f, 1.0f, bits);
	}

	template <class T>
	void Bitstream::writeNormDelta(const T& value, const T& lastValue, uint32_t bits)
	{
		if(value == lastValue)
			write(true);
		else
		{
			write(false);
			writeNorm(value, bits);
		}
	}

	template <class T>
	void Bitstream::readNormDelta(T& value, const T& lastValue, uint32_t bits)
	{
		bool clean;
		read(clean);

		if(clean)
			value = lastValue;
		else
			readNorm(value, bits);
	}


	template <class T>
	void Bitstream::writeRange(const T& value, const T& min, const T& max)
	{
		T range = max - min;
		uint32_t bits = Bitwise::mostSignificantBit(range) + 1;

		T rangeVal = value - min;
		writeBits((QuantType*)&rangeVal, bits);
	}

	template <class T>
	void Bitstream::readRange(T& value, const T& min, const T& max)
	{
		T range = max - min;
		uint32_t bits = Bitwise::mostSignificantBit(range) + 1;

		value = 0;
		readBits((QuantType*)&value, bits);
		value += min;
	}

	template <class T>
	void Bitstream::writeRangeDelta(const T& value, const T& lastValue, const T& min, const T& max)
	{
		if(value == lastValue)
			write(true);
		else
		{
			write(false);
			writeRange(value, min, max);
		}
	}

	template <class T>
	void Bitstream::readRangeDelta(T& value, const T& lastValue, const T& min, const T& max)
	{
		bool clean;
		read(clean);

		if(clean)
			value = lastValue;
		else
			readRange(value, min, max);
	}

	inline void Bitstream::writeRange(float value, float min, float max, uint32_t bits)
	{
		float pct = Math::clamp01((value - min)/(max - min));
		writeNorm(pct, bits);
	}

	inline void Bitstream::readRange(float& value, float min, float max, uint32_t bits)
	{
		float pct;
		readNorm(pct, bits);

		value = min + (max - min) * pct;
	}

	inline void Bitstream::writeRangeDelta(float value, float lastValue, float min, float max, uint32_t bits)
	{
		if(value == lastValue)
			write(true);
		else
		{
			write(false);
			writeRange(value, min, max, bits);
		}
	}

	inline void Bitstream::readRangeDelta(float& value, float lastValue, float min, float max, uint32_t bits)
	{
		bool clean;
		read(clean);

		if(clean)
			value = lastValue;
		else
			readRange(value, min, max, bits);
	}

	inline void Bitstream::skip(int32_t count)
	{
		mCursor = (uint32_t)Math::clamp((int32_t)mCursor + count, 0, (int32_t)mMaxBits);
	}

	inline void Bitstream::seek(uint32_t pos)
	{
		mCursor = std::min(pos, mMaxBits);
	}

	inline void Bitstream::align(uint32_t count)
	{
		if (count == 0)
			return;

		uint32_t bits = count * 8;
		skip(bits - (((mCursor - 1) & (bits - 1)) + 1));
	}

	inline void Bitstream::reallocIfNeeded(uint32_t numBits)
	{
		if (numBits > mMaxBits)
		{
			if (mOwnsMemory)
			{
				// Grow
				const uint32_t newMaxBits = numBits + 4 * BITS_PER_QUANT + numBits / 2;
				realloc(newMaxBits);
			}
			else
			{
				// Caller accessing bits outside of external memory range
				assert(false);
			}
		}
	}

	inline void Bitstream::realloc(uint32_t numBits)
	{
		numBits = Math::divideAndRoundUp(numBits, BITS_PER_QUANT) * BITS_PER_QUANT;

		if (numBits != mMaxBits)
		{
			assert(numBits > mMaxBits);

			const uint32_t numQuants = Math::divideAndRoundUp(numBits, BITS_PER_QUANT);

			// Note: Eventually add support for custom allocators
			auto buffer = bs_allocN<uint8_t>(numQuants);
			if (mData)
			{
				const uint32_t numBytes = Math::divideAndRoundUp(mMaxBits, BITS_PER_QUANT) * BYTES_PER_QUANT;
				memcpy(buffer, mData, numBytes);
				bs_free(mData);
			}

			mData = buffer;
			mMaxBits = numBits;
		}
	}
}
