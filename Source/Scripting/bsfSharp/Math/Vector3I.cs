using System;
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
    /// A three dimensional vector.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Vector3I // Note: Must match C++ class Vector3I
    {
        public static readonly Vector3I Zero = new Vector3I(0, 0, 0);
        public static readonly Vector3I One = new Vector3I(1, 1, 1);
        public static readonly Vector3I XAxis = new Vector3I(1, 0, 0);
        public static readonly Vector3I YAxis = new Vector3I(0, 1, 0);
        public static readonly Vector3I ZAxis = new Vector3I(0, 0, 1);

        public int x;
        public int y;
        public int z;

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
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector3I index.");
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
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector3I index.");
                }
            }
        }

        /// <summary>
        /// Returns a normalized copy of the vector.
        /// </summary>
        public Vector3I Normalized
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
                return (int)MathEx.Sqrt(x * x + y * y + z * z);
            }
        }

        /// <summary>
        /// Returns the squared length of the vector.
        /// </summary>
        public int SqrdLength
        {
            get
            {
                return (x * x + y * y + z * z);
            }
        }

        /// <summary>
        /// Creates a new three dimensional vector.
        /// </summary>
        /// <param name="x">X coordinate.</param>
        /// <param name="y">Y coordinate.</param>
        /// <param name="z">Z coordinate.</param>
        public Vector3I(int x, int y, int z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        /// <summary>
        /// Converts a three dimensional vector into a four dimensional vector. w component will be set to zero.
        /// </summary>
        /// <param name="vec">Vector to convert.</param>
        /// <returns>A new four dimensional vector.</returns>
        public static explicit operator Vector4I(Vector3I vec)
        {
            return new Vector4I(vec.x, vec.y, vec.z, 0);
        }

        public static Vector3I operator +(Vector3I a, Vector3I b)
        {
            return new Vector3I(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3I operator -(Vector3I a, Vector3I b)
        {
            return new Vector3I(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3I operator -(Vector3I v)
        {
            return new Vector3I(-v.x, -v.y, -v.z);
        }

        public static Vector3I operator *(Vector3I a, Vector3I b)
        {
            return new Vector3I(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static Vector3I operator *(Vector3I v, int d)
        {
            return new Vector3I(v.x * d, v.y * d, v.z * d);
        }

        public static Vector3I operator *(int d, Vector3I v)
        {
            return new Vector3I(v.x * d, v.y * d, v.z * d);
        }

        public static Vector3I operator /(Vector3I v, int d)
        {
            return new Vector3I(v.x / d, v.y / d, v.z / d);
        }

        public static bool operator ==(Vector3I lhs, Vector3I rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }

        public static bool operator !=(Vector3I lhs, Vector3I rhs)
        {
            return !(lhs == rhs);
        }

        /// <summary>
        /// Calculates the magnitude of the provided vector.
        /// </summary>
        /// <param name="v">Vector to calculate the magnitude for.</param>
        /// <returns>Magnitude of the vector.</returns>
        public static int Magnitude(Vector3I v)
        {
            return MathEx.Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        /// <summary>
        /// Calculates the squared magnitude of the provided vector.
        /// </summary>
        /// <param name="v">Vector to calculate the magnitude for.</param>
        /// <returns>Squared magnitude of the vector.</returns>
        public static int SqrMagnitude(Vector3I v)
        {
            return (v.x * v.x + v.y * v.y + v.z * v.z);
        }

        /// <summary>
        /// Scales one vector by another.
        /// </summary>
        /// <param name="a">First three dimensional vector.</param>
        /// <param name="b">Second three dimensional vector.</param>
        /// <returns>One vector scaled by another.</returns>
        public static Vector3I Scale(Vector3I a, Vector3I b)
        {
            return new Vector3I(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        /// <summary>
        /// Returns the cross product between two vectors.
        /// </summary>
        /// <param name="lhs">First three dimensional vector.</param>
        /// <param name="rhs">Second three dimensional vector.</param>
        /// <returns>Cross product between two vectors.</returns>
        public static Vector3I Cross(Vector3I lhs, Vector3I rhs)
        {
            return new Vector3I(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
        }

        /// <summary>
        /// Normalizes the provided vector and returns the normalized copy.
        /// </summary>
        /// <param name="value">Vector to normalize.</param>
        /// <returns>Normalized copy of the vector.</returns>
        public static Vector3I Normalize(Vector3I value)
        {
            int num = value.Length;
            if (num > 9.999999E-06)
                return value / num;

            return Zero;
        }

        /// <summary>
        /// Calculates the inner product of the two vectors.
        /// </summary>
        /// <param name="lhs">First three dimensional vector.</param>
        /// <param name="rhs">Second three dimensional vector.</param>
        /// <returns>Inner product between the two vectors.</returns>
        public static int Dot(Vector3I lhs, Vector3I rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        /// <summary>
        /// Calculates the distance between two points.
        /// </summary>
        /// <param name="a">First three dimensional point.</param>
        /// <param name="b">Second three dimensional point.</param>
        /// <returns>Distance between the two points.</returns>
        public static int Distance(Vector3I a, Vector3I b)
        {
            Vector3I vector3 = new Vector3I(a.x - b.x, a.y - b.y, a.z - b.z);
            return MathEx.Sqrt(vector3.x * vector3.x + vector3.y * vector3.y + vector3.z * vector3.z);
        }

        /// <summary>
        /// Scales the components of the vector by specified scale factors.
        /// </summary>
        /// <param name="scale">Scale factors to multiply components by.</param>
        public void Scale(Vector3I scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
        }

        /// <summary>
        /// Normalizes the vector.
        /// </summary>
        public void Normalize()
        {
            int num = Length;
            if (num > 9.999999E-06)
                this /= num;
            else
                this = Zero;
        }

        /// <summary>
        /// Calculates two vectors orthonormal to the first vector.
        /// </summary>
        /// <param name="x">Normalized vector to calculate orthonormal vectors for.</param>
        /// <param name="y">First orthonormal vector.</param>
        /// <param name="z">Second orthonormal vector.</param>
        public static void OrthogonalComplement(Vector3I x, out Vector3I y, out Vector3I z)
        {
            if (MathEx.Abs(x.x) > MathEx.Abs(x.y))
                y = new Vector3I(-x.z, 0, x.x);
            else
                y = new Vector3I(0, x.z, -x.y);

            z = Cross(x, y);

            Orthonormalize(ref x, ref y, ref z);
        }

        /// <summary>
        /// Performs Gram-Schmidt orthonormalization on the specified basis, making all the vectors
        /// normal and orthogonal to each other.
        /// </summary>
        /// <param name="x">First vector to orthogonalize.</param>
        /// <param name="y">Second vector to orthogonalize.</param>
        /// <param name="z">Third vector to orthogonalize.</param>
        public static void Orthonormalize(ref Vector3I x, ref Vector3I y, ref Vector3I z)
        {
            x.Normalize();

            int dot0 = Vector3I.Dot(x, y);
            y -= dot0 * x;
            y.Normalize();

            int dot1 = Vector3I.Dot(y, z);
            dot0 = Vector3I.Dot(x, z);
            z -= dot0 * x + dot1 * y;
            z.Normalize();
        }

        /// <summary>
        /// Returns the maximum of all the vector components as a new vector.
        /// </summary>
        /// <param name="a">First vector.</param>
        /// <param name="b">Second vector.</param>
        /// <returns>Vector consisting of maximum components of the first and second vector.</returns>
        public static Vector3I Max(Vector3I a, Vector3I b)
        {
            return new Vector3I(MathEx.Max(a.x, b.x), MathEx.Max(a.y, b.y), MathEx.Max(a.z, b.z));
        }

        /// <summary>
        /// Returns the minimum of all the vector components as a new vector.
        /// </summary>
        /// <param name="a">First vector.</param>
        /// <param name="b">Second vector.</param>
        /// <returns>Vector consisting of minimum components of the first and second vector.</returns>
        public static Vector3I Min(Vector3I a, Vector3I b)
        {
            return new Vector3I(MathEx.Min(a.x, b.x), MathEx.Min(a.y, b.y), MathEx.Min(a.z, b.z));
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2 ^ z.GetHashCode() >> 2;
        }

        /// <inheritdoc/>
        public override bool Equals(object other)
        {
            if (!(other is Vector3I))
                return false;

            Vector3I vec = (Vector3I)other;
            if (x.Equals(vec.x) && y.Equals(vec.y) && z.Equals(vec.z))
                return true;

            return false;
        }

        /// <inheritdoc/>
        public override string ToString()
        {
            return "(" + x + ", " + y + ", " + z + ")";
        }
    }
}
}
