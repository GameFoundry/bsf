using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float x;
        public float y;
        public float z;

        public static Vector3 zero
        {
            get
            {
                return new Vector3(0.0f, 0.0f, 0.0f);
            }
        }

        public static Vector3 one
        {
            get
            {
                return new Vector3(1.0f, 1.0f, 1.0f);
            }
        }

        public float this[int index]
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
                        throw new IndexOutOfRangeException("Invalid Vector3 index.");
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
                        throw new IndexOutOfRangeException("Invalid Vector3 index.");
                }
            }
        }

        public Vector3 normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        public float magnitude
        {
            get
            {
                return MathEx.Sqrt(x * x + y * y + z * z);
            }
        }

        public float sqrdMagnitude
        {
            get
            {
                return (x * x + y * y + z * z);
            }
        }

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static Vector3 operator+ (Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static Vector3 operator- (Vector3 a, Vector3 b)
        {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static Vector3 operator- (Vector3 v)
        {
            return new Vector3(-v.x, -v.y, -v.z);
        }

        public static Vector3 operator* (Vector3 v, float d)
        {
            return new Vector3(v.x * d, v.y * d, v.z * d);
        }

        public static Vector3 operator* (float d, Vector3 v)
        {
            return new Vector3(v.x * d, v.y * d, v.z * d);
        }

        public static Vector3 operator/ (Vector3 v, float d)
        {
            return new Vector3(v.x / d, v.y / d, v.z / d);
        }

        public static Vector3 Scale(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public static Vector3 Cross(Vector3 lhs, Vector3 rhs)
        {
            return new Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
        }

        public static Vector3 Normalize(Vector3 value)
        {
            float num = Magnitude(value);
            if (num > 9.999999E-06)
                return value / num;
            
            return zero;
        }

        public static float Dot(Vector3 lhs, Vector3 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        public static float Distance(Vector3 a, Vector3 b)
        {
            Vector3 vector3 = new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
            return MathEx.Sqrt(vector3.x * vector3.x + vector3.y * vector3.y + vector3.z * vector3.z);
        }

        public static float Magnitude(Vector3 v)
        {
            return MathEx.Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        public static float SqrMagnitude(Vector3 v)
        {
            return (v.x * v.x + v.y * v.y + v.z * v.z);
        }

        public void Scale(Vector3 scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
        }

        public void Normalize()
        {
            float num = Magnitude(this);
            if (num > 9.999999E-06)
                this /= num;
            else
                this = zero;
        }
    }
}
