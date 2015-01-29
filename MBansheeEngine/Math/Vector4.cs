using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Vector4
    {
        public static readonly Vector4 zero = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        public static readonly Vector4 one = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        public static readonly Vector4 xAxis = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
        public static readonly Vector4 yAxis = new Vector4(0.0f, 1.0f, 0.0f, 0.0f);
        public static readonly Vector4 zAxis = new Vector4(0.0f, 0.0f, 1.0f, 0.0f);

        public float x;
        public float y;
        public float z;
        public float w;

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
                    case 3:
                        return w;
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector4 index.");
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
                        throw new IndexOutOfRangeException("Invalid Vector4 index.");
                }
            }
        }

        public Vector4 normalized
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
                return MathEx.Sqrt(x * x + y * y + z * z + w * w);
            }
        }

        public float sqrdMagnitude
        {
            get
            {
                return (x * x + y * y + z * z + w * w);
            }
        }

        public Vector4(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public static explicit operator Vector3(Vector4 vec)
        {
            return new Vector3(vec.x, vec.y, vec.z);
        }

        public static Vector4 operator+ (Vector4 a, Vector4 b)
        {
            return new Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static Vector4 operator- (Vector4 a, Vector4 b)
        {
            return new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static Vector4 operator- (Vector4 v)
        {
            return new Vector4(-v.x, -v.y, -v.z, -v.w);
        }

        public static Vector4 operator* (Vector4 v, float d)
        {
            return new Vector4(v.x * d, v.y * d, v.z * d, v.w * d);
        }

        public static Vector4 operator* (float d, Vector4 v)
        {
            return new Vector4(v.x * d, v.y * d, v.z * d, v.w * d);
        }

        public static Vector4 operator /(Vector4 v, float d)
        {
            return new Vector4(v.x / d, v.y / d, v.z / d, v.w / d);
        }

        public static bool operator== (Vector4 lhs, Vector4 rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }

        public static bool operator!= (Vector4 lhs, Vector4 rhs)
        {
            return !(lhs == rhs);
        }

        public static Vector4 Scale(Vector4 a, Vector4 b)
        {
            return new Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }

        public static Vector4 Normalize(Vector4 value)
        {
            float num = Magnitude(value);
            if (num > 9.999999E-06)
                return value / num;

            return zero;
        }

        public static float Dot(Vector4 lhs, Vector4 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        public static float Distance(Vector4 a, Vector4 b)
        {
            Vector4 vector4 = new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
            return MathEx.Sqrt(vector4.x * vector4.x + vector4.y * vector4.y + vector4.z * vector4.z + vector4.w * vector4.w);
        }

        public static float Magnitude(Vector4 v)
        {
            return MathEx.Sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        public static float SqrMagnitude(Vector4 v)
        {
            return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        public void Scale(Vector4 scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
            w *= scale.w;
        }

        public void Normalize()
        {
            float num = Magnitude(this);
            if (num > 9.999999e-06f)
                this /= num;
            else
                this = zero;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2 ^ z.GetHashCode() >> 2 ^ w.GetHashCode() >> 1;
        }

        public override bool Equals(object other)
        {
            if (!(other is Vector4))
                return false;

            Vector4 vec = (Vector4)other;
            if (x.Equals(vec.x) && y.Equals(vec.y) && z.Equals(vec.z) && w.Equals(vec.w))
                return true;

            return false;
        }

        public override string ToString()
        {
            return "(" + x + ", " + y + ", " + z + ", " + w + ")";
        }
    }
}
