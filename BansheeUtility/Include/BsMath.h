//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsDegree.h"
#include "BsRadian.h"

namespace BansheeEngine
{
    /**
     * @brief	Utility class providing common scalar math operations.
     */
    class BS_UTILITY_EXPORT Math 
    {
    public:
		static Radian acos(float val);
		static Radian asin(float val);
		static Radian atan(float val) { return Radian(std::atan(val)); }
		static Radian atan2(float y, float x) { return Radian(std::atan2(y,x)); }

        static float cos(const Radian& val) { return (float)std::cos(val.valueRadians()); }
        static float cos(float val) { return (float)std::cos(val); }

        static float sin(const Radian& val) { return (float)std::sin(val.valueRadians()); }
        static float sin(float val) { return (float)std::sin(val); }

		static float tan(const Radian& val) { return (float)std::tan(val.valueRadians()); }
		static float tan(float val) { return (float)std::tan(val); }

		static float sqrt(float val) { return (float)std::sqrt(val); }
        static Radian sqrt(const Radian& val) { return Radian(std::sqrt(val.valueRadians())); }
        static Degree sqrt(const Degree& val) { return Degree(std::sqrt(val.valueDegrees())); }

		static float invSqrt(float val);
		static float sqr(float val) { return val*val; }

		static float pow(float base, float exponent) { return (float)std::pow(base, exponent); }
		static float exp(float val) { return (float)std::exp(val); }

		static float log(float val) { return (float)std::log(val); }
		static float log2(float val) { return (float)(std::log(val)/LOG2); }
		static float logN(float base, float val) { return (float)(std::log(val)/std::log(base)); }

		static float sign(float val);
		static Radian sign(const Radian& val) { return Radian(sign(val.valueRadians())); }
		static Degree sign(const Degree& val) { return Degree(sign(val.valueDegrees())); }

		static float abs(float val) { return float(std::fabs(val)); }
		static Degree abs(const Degree& val) { return Degree(std::fabs(val.valueDegrees())); }
		static Radian abs(const Radian& val) { return Radian(std::fabs(val.valueRadians())); }

		static float ceil(float val) { return (float)std::ceil(val); }
		static int ceilToInt(float val) { return (int)std::ceil(val); }

		static float round(float val) { return (float)std::floor(val + 0.5f); }
		static int roundToInt(float val) { return (int)std::floor(val + 0.5f); }

		static float floor(float val) { return (float)std::floor(val); }
		static int floorToInt(float val) { return (int)std::floor(val); }

		/**
		 * @brief	Clamp a value within an inclusive range.
		 */
		template <typename T>
		static T clamp(T val, T minval, T maxval)
		{
			assert (minval <= maxval && "Invalid clamp range");
			return std::max(std::min(val, maxval), minval);
		}

		/**
		 * @brief	Clamp a value within an inclusive range [0..1].
		 */
		template <typename T>
		static T clamp01(T val)
		{
			return std::max(std::min(val, (T)1), (T)0);
		}

		/**
		 * @brief	Checks is the specified value a power of two. Only works on integer values.
		 */
		template <typename T>
		static bool isPow2(T val)
		{
			return (val & (val - 1)) == 0;
		}

		static bool isNaN(float f)
		{
			return f != f;
		}

        /**
         * @brief	Compare 2 floats, using tolerance for inaccuracies.
         */
        static bool approxEquals(float a, float b, float tolerance = std::numeric_limits<float>::epsilon());

        /**
         * @brief	Calculates the tangent space vector for a given set of positions / texture coords.
         */
        static Vector3 calculateTriTangent(const Vector3& position1, const Vector3& position2, 
			const Vector3& position3, float u1, float v1, float u2, float v2, float u3, float v3);

		/************************************************************************/
		/* 							TRIG APPROXIMATIONS                      	*/
		/************************************************************************/

		/**
		 * @brief	Sine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastSin0(const Radian& val) { return (float)fastASin0(val.valueRadians()); }

		/**
		 * @brief	Sine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastSin0(float val);

		/**
		 * @brief	Sine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastSin0".
		 */
		static float fastSin1(const Radian& val) { return (float)fastASin1(val.valueRadians()); }

		/**
		 * @brief	Sine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastSin0".
		 */
		static float fastSin1(float val);

		/**
		 * @brief	Cosine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastCos0(const Radian& val) { return (float)fastACos0(val.valueRadians()); }

		/**
		 * @brief	Cosine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastCos0(float val);

		/**
		 * @brief	Cosine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastCos0".
		 */
		static float fastCos1(const Radian& val) { return (float)fastACos1(val.valueRadians()); }

		/**
		 * @brief	Cosine function approximation.
		 *
		 * @param	val	Angle in range [0, pi/2].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastCos0".
		 */
		static float fastCos1(float val);

		/**
		 * @brief	Tangent function approximation.
		 *
		 * @param	val	Angle in range [0, pi/4].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastTan0(const Radian& val) { return (float)fastATan0(val.valueRadians()); }

		/**
		 * @brief	Tangent function approximation.
		 *
		 * @param	val	Angle in range [0, pi/4].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastTan0(float val);

		/**
		 * @brief	Tangent function approximation.
		 *
		 * @param	val	Angle in range [0, pi/4].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastTan0".
		 */
		static float fastTan1(const Radian& val) { return (float)fastATan1(val.valueRadians()); }

		/**
		 * @brief	Tangent function approximation.
		 *
		 * @param	val	Angle in range [0, pi/4].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastTan0".
		 */
		static float fastTan1(float val);

		/**
		 * @brief	Inverse sine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastASin0(const Radian& val) { return (float)fastASin0(val.valueRadians()); }

		/**
		 * @brief	Inverse sine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastASin0(float val);

		/**
		 * @brief	Inverse sine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastASin0".
		 */
		static float fastASin1(const Radian& val) { return (float)fastASin1(val.valueRadians()); }

		/**
		 * @brief	Inverse sine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastASin0".
		 */
		static float fastASin1(float val);

		/**
		 * @brief	Inverse cosine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastACos0(const Radian& val) { return (float)fastACos0(val.valueRadians()); }

		/**
		 * @brief	Inverse cosine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastACos0(float val);

		/**
		 * @brief	Inverse cosine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastACos0".
		 */
		static float fastACos1(const Radian& val) { return (float)fastACos1(val.valueRadians()); }

		/**
		 * @brief	Inverse cosine function approximation.
		 *
		 * @param	val	Angle in range [0, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastACos0".
		 */
		static float fastACos1(float val);

		/**
		 * @brief	Inverse tangent function approximation.
		 *
		 * @param	val	Angle in range [-1, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastATan0(const Radian& val) { return (float)fastATan0(val.valueRadians()); }

		/**
		 * @brief	Inverse tangent function approximation.
		 *
		 * @param	val	Angle in range [-1, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 */
		static float fastATan0(float val);

		/**
		 * @brief	Inverse tangent function approximation.
		 *
		 * @param	val	Angle in range [-1, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastATan0".
		 */
		static float fastATan1(const Radian& val) { return (float)fastATan1(val.valueRadians()); }

		/**
		 * @brief	Inverse tangent function approximation.
		 *
		 * @param	val	Angle in range [-1, 1].
		 *
		 * @note	Evaluates trigonometric functions using polynomial approximations.
		 * 			Slightly better (and slower) than "fastATan0".
		 */
		static float fastATan1(float val);

        static const float POS_INFINITY;
        static const float NEG_INFINITY;
        static const float PI;
        static const float TWO_PI;
        static const float HALF_PI;
		static const float DEG2RAD;
		static const float RAD2DEG;
		static const float LOG2;
    };
}
