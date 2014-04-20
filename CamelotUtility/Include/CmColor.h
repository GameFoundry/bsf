#pragma once

#include "CmPrerequisitesUtil.h"

namespace BansheeEngine 
{
    typedef UINT32 RGBA;
    typedef UINT32 ARGB;
    typedef UINT32 ABGR;
	typedef UINT32 BGRA;

	/** 
	 * @brief	Color represented as 4 components, each being a floating point value ranging from
	 *			0 to 1. Color components are Red, Green, Blue and Alpha.
	 */
    class CM_UTILITY_EXPORT Color
    {
    public:
        static const Color ZERO;
        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;

	    explicit Color(float red = 1.0f, float green = 1.0f,
				    float blue = 1.0f, float alpha = 1.0f ) 
					:r(red), g(green), b(blue), a(alpha)
        { }

	    bool operator==(const Color& rhs) const;
	    bool operator!=(const Color& rhs) const;

	    RGBA getAsRGBA(void) const;
	    ARGB getAsARGB(void) const;
		BGRA getAsBGRA(void) const;
	    ABGR getAsABGR(void) const;

        void setAsRGBA(const RGBA val);
        void setAsARGB(const ARGB val);
		void setAsBGRA(const BGRA val);
        void setAsABGR(const ABGR val);

		/** 
		 * @brief	Clamps colour value to the range [0, 1].
		 */
        void saturate()
        {
            if (r < 0)
                r = 0;
            else if (r > 1)
                r = 1;

            if (g < 0)
                g = 0;
            else if (g > 1)
                g = 1;

            if (b < 0)
                b = 0;
            else if (b > 1)
                b = 1;

            if (a < 0)
                a = 0;
            else if (a > 1)
                a = 1;
        }

		/** 
		 * @brief	Clamps colour value to the range [0, 1]. Returned saturated
		 *			color as a copy.
		 */
        Color saturateCopy() const
        {
            Color ret = *this;
            ret.saturate();
            return ret;
        }

		inline float operator[] (const UINT32 i) const
		{
			assert(i < 4);

			return *(&r+i);
		}

		inline float& operator[] (const UINT32 i)
		{
			assert(i < 4);

			return *(&r+i);
		}

		/** 
		 * @brief	Pointer accessor for direct copying.
		 */
		inline float* ptr()
		{
			return &r;
		}

		/** 
		 * @brief	Pointer accessor for direct copying.
		 */
		inline const float* ptr() const
		{
			return &r;
		}

        inline Color operator+ (const Color& rhs) const
        {
            Color kSum;

            kSum.r = r + rhs.r;
            kSum.g = g + rhs.g;
            kSum.b = b + rhs.b;
            kSum.a = a + rhs.a;

            return kSum;
        }

        inline Color operator- ( const Color& rhs) const
        {
            Color kDiff;

            kDiff.r = r - rhs.r;
            kDiff.g = g - rhs.g;
            kDiff.b = b - rhs.b;
            kDiff.a = a - rhs.a;

            return kDiff;
        }

        inline Color operator* (const float rhs) const
        {
            Color kProd;

            kProd.r = rhs*r;
            kProd.g = rhs*g;
            kProd.b = rhs*b;
            kProd.a = rhs*a;

            return kProd;
        }

        inline Color operator* (const Color& rhs) const
        {
            Color kProd;

            kProd.r = rhs.r * r;
            kProd.g = rhs.g * g;
            kProd.b = rhs.b * b;
            kProd.a = rhs.a * a;

            return kProd;
        }

        inline Color operator/ (const Color& rhs) const
        {
            Color kProd;

            kProd.r = rhs.r / r;
            kProd.g = rhs.g / g;
            kProd.b = rhs.b / b;
            kProd.a = rhs.a / a;

            return kProd;
        }

        inline Color operator/ (const float rhs) const
        {
            assert(rhs != 0.0f);

            Color kDiv;

            float fInv = 1.0f / rhs;
            kDiv.r = r * fInv;
            kDiv.g = g * fInv;
            kDiv.b = b * fInv;
            kDiv.a = a * fInv;

            return kDiv;
        }

        inline friend Color operator* (const float lhs, const Color& rhs)
        {
            Color result;

            result.r = lhs * rhs.r;
            result.g = lhs * rhs.g;
            result.b = lhs * rhs.b;
            result.a = lhs * rhs.a;

            return result;
        }

        inline Color& operator+= (const Color& rhs)
        {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            a += rhs.a;

            return *this;
        }

        inline Color& operator-= (const Color& rhs)
        {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            a -= rhs.a;

            return *this;
        }

        inline Color& operator*= (const float rhs)
        {
            r *= rhs;
            g *= rhs;
            b *= rhs;
            a *= rhs;

            return *this;
        }

        inline Color& operator/= (const float rhs)
        {
            assert(rhs != 0.0f);

            float fInv = 1.0f / rhs;

            r *= rhs;
            g *= rhs;
            b *= rhs;
            a *= rhs;

            return *this;
        }

		/** 
		 * @brief	Set a colour value from Hue, Saturation and Brightness.
		 *
		 * @param hue				Hue value, scaled to the [0,1] range.
		 * @param saturation		Saturation level, [0,1].
		 * @param brightness		Brightness level, [0,1].
		 */
		void setHSB(float hue, float saturation, float brightness);

		/** 
		 * @brief Convert the current color to Hue, Saturation and Brightness values. 
		 * 
		 * @param hue			Output hue value, scaled to the [0,1] range.
		 * @param saturation	Output saturation level, [0,1].
		 * @param brightness	Output brightness level, [0,1].
		 */
		void getHSB(float* hue, float* saturation, float* brightness) const;

		float r, g, b, a;
    };

	CM_ALLOW_MEMCPY_SERIALIZATION(Color);
}
