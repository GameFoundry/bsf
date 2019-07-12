#include "./BsBitstream.h"

namespace bs 
{
	uint32_t Bitstream::BYTES_PER_QUANT = sizeof(QuantType);
	uint32_t Bitstream::BITS_PER_QUANT = BYTES_PER_QUANT * 8;
	uint32_t Bitstream::BITS_PER_QUANT_LOG2 = Bitwise::bitsLog2(BITS_PER_QUANT);
}  // namespace bs
