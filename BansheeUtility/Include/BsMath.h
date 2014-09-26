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
         * @brief	Compare 2 doubles, using tolerance for inaccuracies.
         */
		static bool approxEquals(double a, double b, double tolerance = std::numeric_limits<double>::epsilon());

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

		/**
		 * @brief	Interpolates between min and max. Returned value is in
		 *			[0, 1] range where min = 0, max = 1 and 0.5 is the average
		 *			of min and max.
		 */
		template <typename T>
		static float lerp01(T val, T min, T max)
		{
			return clamp01((val - min) / std::max(max - min, 0.0001F));
		}

		/**
		 * @brief	Solves the linear equation with the parameters A, B.
		 *			Returns number of roots found and the roots themselves will
		 *			be output in the "roots" array.
		 *
		 * @param	roots	Must be at least size of 1.
		 *
		 * @note	Only returns real roots.
		 */
		template <typename T>
		static UINT32 solveLinear(T A, T B, T* roots)
		{
			if (!approxEquals(B, (T)0))
			{
				roots[0] = -A / B;
				return 1;
			}
			else if (approxEquals(A, (T)0))
			{
				roots[0] = 0.0f;
				return 1;
			}

			return 0;
		}

		/**
		 * @brief	Solves the quadratic equation with the parameters A, B, C.
		 *			Returns number of roots found and the roots themselves will
		 *			be output in the "roots" array.
		 *
		 * @param	roots	Must be at least size of 2.
		 *
		 * @note	Only returns real roots.
		 */
		template <typename T>
		static UINT32 solveQuadratic(T A, T B, T C, T* roots)
		{
			if (!approxEquals(C, (T)0))
			{
				T discr = B * B - 4 * A * C;
				if (discr > std::numeric_limits<T>::epsilon())
				{
					float temp = ((T)0.5) / C;
					discr = std::sqrt(discr);

					roots[0] = temp * (-B - discr);
					roots[1] = temp * (-B + discr);

					return 2;
				}
				else if (discr < -std::numeric_limits<T>::epsilon())
				{
					return 0;
				}
				else
				{
					roots[0] = ((T)-0.5) * (B / C);
					return 1;
				}
			}
			else
			{
				return solveLinear(A, B, roots);
			}
		}

		/**
		 * @brief	Solves the cubic equation with the parameters A, B, C, D.
		 *			Returns number of roots found and the roots themselves will
		 *			be output in the "roots" array.
		 *
		 * @param	roots	Must be at least size of 3.
		 *
		 * @note	Only returns real roots.
		 */
		template <typename T>
		static UINT32 solveCubic(T A, T B, T C, T D, T* roots)
		{
			static const T THIRD = (1 / (T)3);

			if (!approxEquals(D, (T)0))
			{
				T invD = 1 / D;
				T k0 = A * invD;
				T k1 = B * invD;
				T k2 = C * invD;

				T offset = THIRD * k2;
				T a = k1 - k2 * offset;
				T b = k0 + k2 * (2 * k2 * k2 - 9 * k1) * (1 / (T)27);
				T halfB = ((T)0.5) * b;

				T discr = halfB * halfB + a * a * a * (1 / (T)27);
				if (discr > std::numeric_limits<T>::epsilon())
				{
					discr = std::sqrt(discr);
					T temp = -halfB + discr;
					if (temp >= (T)0)
						roots[0] = pow(temp, THIRD);
					else
						roots[0] = -pow(-temp, THIRD);

					temp = -halfB - discr;
					if (temp >= 0)
						roots[0] += pow(temp, THIRD);
					else
						roots[0] -= -pow(-temp, THIRD);

					roots[0] -= offset;
					return 1;
				}
				else if (discr < -std::numeric_limits<T>::epsilon())
				{
					T sqrtThree = std::sqrt((T)3);
					T dist = sqrt(-THIRD * a);
					T angle = THIRD * atan2(std::sqrt(-discr), -halfB).valueRadians();
					T angleCos = cos(angle);
					T angleSin = sin(angle);

					roots[0] = 2 * dist * angleCos - offset;
					roots[1] = -dist * (angleCos + sqrtThree * angleSin) - offset;
					roots[2] = -dist * (angleCos - sqrtThree * angleSin) - offset;

					return 3;
				}
				else
				{
					T temp;
					if (halfB >= (T)0)
						temp = -pow(halfB, THIRD);
					else
						temp = pow(-halfB, THIRD);

					roots[0] = 2 * temp - offset;
					roots[1] = -temp - offset;
					roots[2] = roots[1];

					return 3;
				}
			}
			else
			{
				return solveQuadratic(A, B, C, roots);
			}
		}

		/**
		 * @brief	Solves the quartic equation with the parameters A, B, C, D, E.
		 *			Returns number of roots found and the roots themselves will
		 *			be output in the "roots" array.
		 *
		 * @param	roots	Must be at least size of 4.
		 *
		 * @note	Only returns real roots.
		 */
		template <typename T>
		static UINT32 solveQuartic(T A, T B, T C, T D, T E, T* roots)
		{
			if (!approxEquals(E, (T)0))
			{
				T invE = 1 / E;
				T k0 = A * invE;
				T k1 = B * invE;
				T k2 = C * invE;
				T k3 = D * invE;

				T r0 = k0 * (4 * k2 - k3 * k3) - k1 * k1;
				T r1 = k3 * k1 - 4 * k0;
				T r2 = -k2;
				solveCubic(r0, r1, r2, (T)1, roots);
				T y = roots[0];

				UINT32 numRoots = 0;
				T discr = ((T)0.25) * k3 * k3 - k2 + y;
				if (discr > std::numeric_limits<T>::epsilon())
				{
					T r = sqrt(discr);
					T t1 = ((T)0.75) * k3 * k3 - r * r - 2*k2;
					T t2 = (k3 * k2 - 2 * k1 - ((T)0.25) * k3 * k3 * k3) / r;

					T tPlus = t1 + t2;
					if (tPlus >= ((T)0))
					{
						T d = std::sqrt(tPlus);
						roots[0] = ((T)-0.25) * k3 + ((T)0.5) * (r + d);
						roots[1] = ((T)-0.25) * k3 + ((T)0.5) * (r - d);

						numRoots += 2;
					}

					T tMinus = t1 - t2;
					if (tMinus >= ((T)0))
					{
						T e = std::sqrt(tMinus);
						roots[numRoots++] = ((T)-0.25) * k3 + ((T)0.5) * (e - r);
						roots[numRoots++] = ((T)-0.25) * k3 - ((T)0.5) * (e + r);
					}
				}
				else if (discr < -std::numeric_limits<T>::epsilon())
				{
					numRoots = 0;
				}
				else
				{
					T t2 = y * y - 4 * k0;
					if (t2 >= ((T)0))
					{
						t2 = 2 * std::sqrt(t2);
						T t1 = ((T)0.75) * k3 * k3 - 2 * k2;

						T tPlus = t1 + t2;
						if (tPlus >= ((T)0))
						{
							T d = std::sqrt(tPlus);
							roots[0] = ((T)-0.25) * k3 + ((T)0.5) * d;
							roots[1] = ((T)-0.25) * k3 + ((T)0.5) * d;

							numRoots += 2;
						}

						T tMinus = t1 - t2;
						if (tMinus >= ((T)0))
						{
							T e = std::sqrt(tMinus);
							roots[numRoots++] = ((T)-0.25) * k3 + ((T)0.5) * e;
							roots[numRoots++] = ((T)-0.25) * k3 - ((T)0.5) * e;
						}
					}
				}

				return numRoots;
			}
			else
			{
				return solveCubic(A, B, C, D, roots);
			}
		}

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
