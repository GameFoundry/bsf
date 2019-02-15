﻿using System;
using System.Runtime.InteropServices;

#if IS_B3D
namespace BansheeEngine
#else
namespace bs
#endif
{
    /** @addtogroup Math
     *  @{
     */

    /// <summary>
    /// A four dimensional vector with a homogeneous w coordinate.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Vector4I // Note: Must match C++ class Vector4I
    {
        public static readonly Vector4I Zero = new Vector4I(0, 0, 0, 0);
        public static readonly Vector4I One = new Vector4I(1, 1, 1, 1);
        public static readonly Vector4I XAxis = new Vector4I(1, 0, 0, 0);
        public static readonly Vector4I YAxis = new Vector4I(0, 1, 0, 0);
        public static readonly Vector4I ZAxis = new Vector4I(0, 0, 1, 0);

        public int x;
        public int y;
        public int z;
        public int w;

        /// <summary>
        /// Accesses a specific component of the vector.
        /// </summary>
        /// <param name="index">Index of the component.</param>
        /// <returns>Value of the specific component.</returns>
        public int this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    case 3:
                        return w;
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector4I index.");
                }
            }

            set
            {
                switch (index)
                {
                    case 0:
                        x = value;
                        break;
                    case 1:
                        y = value;
                        break;
                    case 2:
                        z = value;
                        break;
                    case 3:
                        w = value;
                        break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector4I index.");
                }
            }
        }

        /// <summary>
        /// Returns a normalized copy of the vector.
        /// </summary>
        public Vector4I Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        /// <summary>
        /// Returns the length of the vector.
        /// </summary>
        public int Length
        {
            get
            {
                return MathEx.Sqrt(x * x + y * y + z * z + w * w);
            }
        }

        /// <summary>
        /// Returns the squared length of the vector.
        /// </summary>
        public int SqrdLength
        {
            get
            {
                return (x * x + y * y + z * z + w * w);
            }
        }

        /// <summary>
        /// Creates a new four dimensional vector.
        /// </summary>
        /// <param name="x">X coordinate.</param>
        /// <param name="y">Y coordinate.</param>
        /// <param name="z">Z coordinate.</param>
        /// <param name="w">Homogeneous W coordinate.</param>
        public Vector4I(int x, int y, int z, int w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        /// <summary>
        /// Converts a homogenous vector into a three dimensional vector. w component is discarded.
        /// </summary>
        /// <param name="vec">Vector to convert.</param>
        /// <returns>A new three dimensional vector.</returns>
        public static explicit operator Vector3I(Vector4I vec)
        {
            return new Vector3I(vec.x, vec.y, vec.z);
        }

        public static Vector4I operator +(Vector4I a, Vector4I b)
        {
            return new Vector4I(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static Vector4I operator -(Vector4I a, Vector4I b)
        {
            return new Vector4I(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static Vector4I operator -(Vector4I v)
        {
            return new Vector4I(-v.x, -v.y, -v.z, -v.w);
        }

        public static Vector4I operator *(Vector4I a, Vector4I b)
        {
            return new Vector4I(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        public static Vector4I operator *(Vector4I v, int d)
        {
            return new Vector4I(v.x * d, v.y * d, v.z * d, v.w * d);
        }

        public static Vector4I operator *(int d, Vector4I v)
        {
            return new Vector4I(v.x * d, v.y * d, v.z * d, v.w * d);
        }

        public static Vector4I operator /(Vector4I v, int d)
        {
            return new Vector4I(v.x / d, v.y / d, v.z / d, v.w / d);
        }

        public static bool operator ==(Vector4I lhs, Vector4I rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }

        public static bool operator !=(Vector4I lhs, Vector4I rhs)
        {
            return !(lhs == rhs);
        }

        /// <summary>
        /// Scales one vector by another.
        /// </summary>
        /// <param name="a">First four dimensional vector.</param>
        /// <param name="b">Second four dimensional vector.</param>
        /// <returns>One vector scaled by another.</returns>
        public static Vector4I Scale(Vector4I a, Vector4I b)
        {
            return new Vector4I(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        /// <summary>
        /// Normalizes the provided vector and returns the normalized copy.
        /// </summary>
        /// <param name="value">Vector to normalize.</param>
        /// <returns>Normalized copy of the vector.</returns>
        public static Vector4I Normalize(Vector4I value)
        {
            int num = Magnitude(value);
            if (num > 9.999999E-06)
                return value / num;

            return Zero;
        }

        /// <summary>
        /// Calculates the inner product of the two vectors.
        /// </summary>
        /// <param name="lhs">First four dimensional vector.</param>
        /// <param name="rhs">Second four dimensional vector.</param>
        /// <returns>Inner product between the two vectors.</returns>
        public static int Dot(Vector4I lhs, Vector4I rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        /// <summary>
        /// Calculates the distance between two points.
        /// </summary>
        /// <param name="a">First four dimensional point.</param>
        /// <param name="b">Second four dimensional point.</param>
        /// <returns>Distance between the two points.</returns>
        public static int Distance(Vector4I a, Vector4I b)
        {
            Vector4I vector4 = new Vector4I(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
            return MathEx.Sqrt(vector4.x * vector4.x + vector4.y * vector4.y + vector4.z * vector4.z + vector4.w * vector4.w);
        }

        /// <summary>
        /// Calculates the magnitude of the provided vector.
        /// </summary>
        /// <param name="v">Vector to calculate the magnitude for.</param>
        /// <returns>Magnitude of the vector.</returns>
        public static int Magnitude(Vector4I v)
        {
            return MathEx.Sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        /// <summary>
        /// Calculates the squared magnitude of the provided vector.
        /// </summary>
        /// <param name="v">Vector to calculate the magnitude for.</param>
        /// <returns>Squared magnitude of the vector.</returns>
        public static int SqrMagnitude(Vector4I v)
        {
            return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        /// <summary>
        /// Scales the components of the vector by specified scale factors.
        /// </summary>
        /// <param name="scale">Scale factors to multiply components by.</param>
        public void Scale(Vector4I scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
            w *= scale.w;
        }

        /// <summary>
        /// Normalizes the vector.
        /// </summary>
        public void Normalize()
        {
            int num = Magnitude(this);
            if (num > 9.999999e-06f)
                this /= num;
            else
                this = Zero;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2 ^ z.GetHashCode() >> 2 ^ w.GetHashCode() >> 1;
        }

        /// <inheritdoc/>
        public override bool Equals(object other)
        {
            if (!(other is Vector4I))
                return false;

            Vector4I vec = (Vector4I)other;
            if (x.Equals(vec.x) && y.Equals(vec.y) && z.Equals(vec.z) && w.Equals(vec.w))
                return true;

            return false;
        }

        /// <inheritdoc/>
        public override string ToString()
        {
            return "(" + x + ", " + y + ", " + z + ", " + w + ")";
        }
    }
}

