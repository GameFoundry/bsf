//************************************ bs::framework - Copyright 2018-2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;

namespace bs
{
    /** @addtogroup Math
	 *  @{
	 */

    /// <summary>
    /// Generates pseudo random numbers using the Xorshift128 algorithm. Suitable for high performance requirements.
    /// </summary>
    public class Random
    {
		private uint[] seed = new uint[4];

        /// <summary>
        /// Initializes a new generator using the specified seed.
        /// </summary>
        /// <param name="seed">Seed value.</param>
        public Random(uint seed = 0)
        {
            SetSeed(seed);
        }

        /// <summary>
        /// Changes the seed of the generator to the specified value.
        /// </summary>
        /// <param name="seed"></param>
        public void SetSeed(uint seed)
        {
            this.seed[0] = seed;
            this.seed[1] = seed * 0x72e0447c + 1; // Arbitrary random numbers
            this.seed[2] = seed * 0x352ad225 + 1;
            this.seed[3] = seed * 0x03c3629f + 1;
        }

        /// <summary>
        /// Returns a random value in range [0, max].
        /// </summary>
        /// <returns></returns>
        public uint Get()
		{
            // Using xorshift128 algorithm
            uint t = this.seed[3];
            t ^= t << 11;
			t ^= t >> 8;

			this.seed[3] = this.seed[2];
			this.seed[2] = this.seed[1];
			this.seed[1] = this.seed[0];

			uint s = this.seed[0];
            t ^= s;
			t ^= s >> 19;

			this.seed[0] = t;
			return t;
		}

        /// <summary>
        /// Returns a random value in range [min, max].
        /// </summary>
        /// <param name="min">Minimum value.</param>
        /// <param name="max">Maximum value.</param>
        /// <returns>Random value.</returns>
        public uint GetRange(uint min, uint max)
		{
            // Note: Not using modulo for performance
            uint range = max - min + 1;

            float DELTA = 0e-5f;
			return min + (uint) (GetUNorm() * ((float) range - DELTA));
		}

        /// <summary>
        /// Returns a random value in range [0, 1].
        /// </summary>
        /// <returns>Random value.</returns>
        public float GetUNorm()
		{
			// Mask first 23 bits and divide by 2^23-1
			return (float)(Get() & 0x007FFFFF) / 8388607.0f;
		}

        /// <summary>
        /// Returns a random value in range [-1, 1].
        /// </summary>
        /// <returns>Random value.</returns>
        public float GetSNorm()
		{
			return GetUNorm() * 2.0f - 1.0f;
		}

        /// <summary>
        /// Returns a random unit vector in three dimensions.
        /// </summary>
        /// <returns>Random unit vector.</returns>
        public Vector3 GetUnitVector()
		{
			// Pick a random number on a unit cube and use the result only if squared size less than 1. This is faster
			// than most other methods, and generally not many iterations are required to get the required vector.

			Vector3 output = Vector3.Default();
            float sqrdSize;

			do
			{
				output.x = GetSNorm();
                output.y = GetSNorm();
                output.z = GetSNorm();
                sqrdSize = Vector3.SqrMagnitude(output);
				
			} while (sqrdSize > 1.0f || sqrdSize < 0.001f);

			return Vector3.Normalize(output);
		}

        /// <summary>
        /// Returns a random unit vector in two dimensions.
        /// </summary>
        /// <returns>Random unit vector</returns>
        public Vector2 GetUnitVector2D()
		{
			// Pick a random number on a unit square and use the result only if squared size less than 1. This is faster
			// than most other methods, and generally not many iterations are required to get the required vector.

			Vector2 output = Vector2.Default();
            float sqrdSize;

			do
			{
				output.x = GetSNorm();
                output.y = GetSNorm();
                sqrdSize = Vector2.SqrMagnitude(output);
				
			} while (sqrdSize > 1.0f || sqrdSize< 0.001f);

			return Vector2.Normalize(output);
		}

        /// <summary>
        /// Returns a random point inside a unit sphere.
        /// </summary>
        /// <returns>Random point.</returns>
        public Vector3 getPointInSphere()
		{
			Vector3 dir = GetUnitVector();
			return dir * MathEx.Pow(GetUNorm(), 1.0f / 3.0f);
        }

		/// <summary>
        /// Returns a random point inside the specified range in a sphere shell of unit radius, with the specified
		/// thickness, in range[0, 1]. Thickness of 0 will generate points on the sphere surface, while thickness of 1
		/// will generate points within the entire sphere volume. Intermediate values represent the shell, which is a volume
		/// between two concentric spheres.
        /// </summary>
        /// <param name="thickness">Thickness value.</param>
        /// <returns>Random point.</returns>
        public Vector3 GetPointInSphereShell(float thickness)
		{
			float minRadius = 1.0f - thickness;

            Vector3 dir = GetUnitVector();
			return dir * (minRadius + thickness * MathEx.Pow(GetUNorm(), 1.0f / 3.0f));
		}

        /// <summary>
        /// Returns a random point inside a unit circle.
        /// </summary>
        /// <returns>Random point.</returns>
        public Vector2 getPointInCircle()
		{
			Vector2 dir = GetUnitVector2D();
			return dir * MathEx.Pow(GetUNorm(), 1.0f / 2.0f);
        }

		/// <summary>
        /// Returns a random point inside the specified range in a circle shell of unit radius, with the specified
		/// thickness, in range[0, 1]. Thickness of 0 will generate points on the circle edge, while thickness of 1 will
        /// generate points within the entire circle surface. Intermediate values represent the shell, which is the surface
		/// between two concentric circles.
        /// </summary>
        /// <param name="thickness">Thickness value.</param>
        /// <returns>Random point.</returns>
        public Vector2 GetPointInCircleShell(float thickness)
		{
			float minRadius = 1.0f - thickness;

            Vector2 dir = GetUnitVector2D();
			return dir * (minRadius + thickness * MathEx.Pow(GetUNorm(), 1.0f / 2.0f));
		}

        /// <summary>
        /// Returns a random point on a unit arc with the specified length (angle). Angle of 360 represents a circle.
        /// </summary>
        /// <param name="angle">Angle value.</param>
        /// <returns>Random point.</returns>
        public Vector2 GetPointInArc(Degree angle)
		{
			float val = GetUNorm() * angle.Radians;
			return new Vector2(MathEx.Cos(val), MathEx.Sin(val));
        }

		/// <summary>
        /// Returns a random point inside the specified range in an arc shell of unit radius, with the specified 
		/// length(angle) and thickness in range[0, 1]. Angle of 360 represents a circle shell.Thickness of 0 will
        /// generate points on the arc edge, while thickness of 1 will generate points on the entire arc 'slice'. 
		/// Intermediate vlaues represent the shell, which is the surface between two concentric circles.
        /// </summary>
        /// <param name="angle">Angle value.</param>
        /// <param name="thickness">Thickness value.</param>
        /// <returns>Random point.</returns>
        public Vector2 GetPointInArcShell(Degree angle, float thickness)
		{
			float minRadius = 1.0f - thickness;

            float val = GetUNorm() * angle.Radians;
            Vector2 dir = new Vector2(MathEx.Cos(val), MathEx.Sin(val));

			return dir * (minRadius + thickness * MathEx.Pow(GetUNorm(), 1.0f / 2.0f));
		}

        /// <summary>
        /// Returns a random set of Barycentric coordinates that may be used for generating random points on a triangle.
        /// </summary>
        /// <returns>Random set.</returns>
        public Vector3 GetBarycentric()
		{
			float u = GetUNorm();
            float v = GetUNorm();

			if((u + v) > 1.0f)
			{
				u = 1.0f - u;
				v = 1.0f - v;
			}

			float w = 1.0f - u - v;
			return new Vector3(u, v, w);
		}
    }

    /** @} */
}
