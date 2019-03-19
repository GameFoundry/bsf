//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;

namespace bs
{
    /** @addtogroup General
     *  @{
     */

    /// <summary>
    /// Class for manipulating bit patterns.
    /// </summary>
    public class Bitwise
    {
        /// <summary>
        /// Returns the power-of-two number greater or equal to the provided value.
        /// </summary>
        /// <param name="n">Parameter to take power-of-two value of.</param>
        /// <returns>Next power-of-two of the provided value</returns>
        public static UInt32 NextPow2(UInt32 n)
        {
            --n;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;
            ++n;
            return n;
        }

        /// <summary>
        /// Returns the power-of-two number closest to the provided value.
        /// </summary>
        /// <param name="n">Parameter to take power-of-two value of.</param>
        /// <returns>Closest the power-of-two number</returns>
        public static UInt32 ClosestPow(UInt32 n)
        {
            UInt32 next = NextPow2(n);

            UInt32 prev = next >> 1;
            if (n - prev < next - n)
                return prev;

            return next;
        }

        /// <summary>
        /// Returns modular exponentiation for integers.
        /// </summary>
        /// <param name="val1">Base value.</param>
        /// <param name="val2">Exponent value.</param>
        /// <param name="t">Third value.</param>
        /// <returns>modular exponentiation of the provided values.</returns>
        public static UInt32 ModPow(UInt32 val1, UInt32 val2, UInt32 t)
        {
            UInt32 res = 1;
            bool b = Convert.ToBoolean(val2) & Convert.ToBoolean(1);
            
            while (val2 != 0)
            {
                if (b)
                    res = (res * val1) % t;

                val2 >>= 1;
                val1 = (val1 * val1) % t;
            }

            return res;
        }

        /// <summary>
        /// Determines whether the number is power-of-two or not.
        /// </summary>
        /// <param name="n">Provided value.</param>
        /// <returns>True if the number is a power of two, false otherwise.</returns>
        public static bool IsPow2(UInt32 n)
        {
            return (n & (n - 1)) == 0;
        }

        /// <summary>
        /// Converts floating point value in range [0, 1] to an unsigned integer of a certain number of bits. 
        /// Works for any value of bits between 0 and 31.
        /// </summary>
        /// <param name="value">Value parameter.</param>
        /// <param name="bits">Bits parameter.</param>
        /// <returns>Converted floating point value to unsigned integer value.</returns>
        public static int UnormToUint(float value, int bits)
        {
            if (value <= 0.0f)
                return 0;

            if (value >= 1.0f)
                return (1 << bits) - 1;

            return MathEx.RoundToInt(value * (1 << bits));
        }

        /// <summary>
        /// Converts floating point value in range [-1, 1] to an unsigned integer of a certain number of bits. 
        /// Works for any value of bits between 0 and 31.
        /// </summary>
        /// <param name="value">Value parameter.</param>
        /// <param name="bits">Bits parameter.</param>
        /// <returns>Converted floating point value to unsigned integer value.</returns>
        public static int SnormToUint(float value, int bits)
        {
            return UnormToUint((value + 1.0f) * 0.5f, bits);
        }

        /// <summary>
        /// Converts an unsigned integer to a floating point in range [0, 1].
        /// </summary>
        /// <param name="value">Value parameter.</param>
        /// <param name="bits">Bits parameter.</param>
        /// <returns>Converted unsigned integer value to floating point.</returns>
        public static float UintToUnorm(UInt32 value, int bits)
        {
            return (float)value / (float)((1 << bits) - 1);
        }

        /// <summary>
        /// Converts an unsigned integer to a floating point in range [-1, 1].
        /// </summary>
        /// <param name="value">Value parameter.</param>
        /// <param name="bits">Bits parameter.</param>
        /// <returns>Converted unsigned int to floating point.</returns>
        public static float UintToSnorm(UInt32 value)
        {
            return UintToUnorm(value, 8) * 2.0f - 1.0f;
        }

        /// <summary>
        /// Interpolates between two values using the @p t parameter. All parameters must be in [0, 255] range. 
        /// When @p t is zero, @p from value will be returned, and when it is 255 @p to value will be returned, 
        /// and interpolation between @p from and @p to will occurr for in-between values.
        /// </summary>
        /// <param name="from">From parameter.</param>
        /// <param name="to">To parameter.</param>
        /// <param name="t">Blend parameter.</param>
        /// <returns>Byte interpolation.</returns>
        public static UInt32 LerpByte(UInt32 from, UInt32 to, UInt32 t)
        {
            return (from + (((to - from) * t) >> 8)) & 0xFF;
        }

        /// <summary>
        /// Interpolates between two values using the @p t parameter. All parameters must be in [0, 65536] range. 
        /// When @p t is zero, @p from value will be returned, and when it is 65536 @p to value will be returned, 
        /// and interpolation between @p from and @p to will occurr for in-between values.
        /// </summary>
        /// <param name="from">From parameter.</param>
        /// <param name="to">To parameter.</param>
        /// <param name="t">Blend parameter.</param>
        /// <returns>Word interpolation</returns>
        public static UInt32 LerpWord(UInt32 from, UInt32 to, UInt32 t)
        {
            return (from + (((to - from) * t) >> 16)) & 0xFFFF;
        }

        /// <summary>
        /// Determines the position of the @p val parameter in the [from, to] range, 
        /// returned in [0, 255] range where 0 is returned if @p val is less or equal than @p from, 
        /// and 255 is returned if @p val is equal to greater to @p to, 
        /// and in-between values returned accordingly.All values must be in [0, 255] range.
        /// </summary>
        /// <param name="from">From parameter.</param>
        /// <param name="to">To parameter.</param>
        /// <param name="val">Blend parameter.</param>
        /// <returns>Byte inversion lerp.</returns>
        public static UInt32 InvLerpByte(UInt32 from, UInt32 to, UInt32 val)
        {
            return ((val - from) << 8) / MathEx.Max(to - from, 1u);
        }

        /// <summary>
        /// * Determines the position of the @p val parameter in the [from, to] range, 
        /// returned in [0, 65536] range where 0 is returned if @p val is less or equal than @p from, 
        /// and 65536 is returned if @p val is equal to greater to @p to, 
        /// and in-between values returned accordingly.All values must be in [0, 65536] range.
        /// </summary>
        /// <param name="from">From parameter.</param>
        /// <param name="to">To parameter.</param>
        /// <param name="val">Blend parameter.</param>
        /// <returns>Word inversion lerp.</returns>
        public static UInt32 InvLerpWord(UInt32 from, UInt32 to, UInt32 val)
        {
            return ((val - from) << 16) / MathEx.Max(to - from, 1u);
        }

        /// <summary>
        /// Converts a float in range [-1,1] into an unsigned 8-bit integer.
        /// </summary>
        /// <param name="v">Value parameter.</param>
        /// <returns>Coverted floating point to unsigned 8-bit integer.</returns>
        public static byte Quantize8BitSigned(float v)
        {
            return Quantize8BitUnsigned(v * 0.5f + 0.5f);
        }

        /// <summary>
        /// Converts a float in range [0,1] into an unsigned 8-bit integer.
        /// </summary>
        /// <param name="v">Value parameter.</param>
        /// <returns>Coverted floating point to unsigned 8-bit integer.</returns>
        public static byte Quantize8BitUnsigned(float v)
        {
            return (byte)(v * 255.999f);
        }
    }

    /** @} */
}
