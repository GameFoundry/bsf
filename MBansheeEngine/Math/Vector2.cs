using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Vector2
    {
        public float x;
        public float y;

        public static readonly Vector2 zero = new Vector2(0.0f, 0.0f);
        public static readonly Vector2 one = new Vector2(1.0f, 1.0f);
        public static readonly Vector2 xAxis = new Vector2(1.0f, 0.0f);
        public static readonly Vector2 yAxis = new Vector2(0.0f, 1.0f);

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
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector2 index.");
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
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector2 index.");
                }
            }
        }

        public Vector2 normalized
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
                return MathEx.Sqrt(x * x + y * y);
            }
        }

        public float sqrdMagnitude
        {
            get
            {
                return (x * x + y * y);
            }
        }

        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public static Vector2 operator+ (Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + b.x, a.y + b.y);
        }

        public static Vector2 operator- (Vector2 a, Vector2 b)
        {
            return new Vector2(a.x - b.x, a.y - b.y);
        }

        public static Vector2 operator- (Vector2 v)
        {
            return new Vector2(-v.x, -v.y);
        }

        public static Vector2 operator* (Vector2 v, float d)
        {
            return new Vector2(v.x * d, v.y * d);
        }

        public static Vector2 operator* (float d, Vector2 v)
        {
            return new Vector2(v.x * d, v.y * d);
        }

        public static Vector2 operator/ (Vector2 v, float d)
        {
            return new Vector2(v.x / d, v.y / d);
        }

        public static bool operator== (Vector2 lhs, Vector2 rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }

        public static bool operator!= (Vector2 lhs, Vector2 rhs)
        {
            return !(lhs == rhs);
        }

        public static Vector2 Scale(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * b.x, a.y * b.y);
        }

        public static Vector2 Normalize(Vector2 value)
        {
            float num = Magnitude(value);
            if (num > 9.999999E-06f)
                return value / num;

            return zero;
        }

        public static float Dot(Vector2 lhs, Vector2 rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y;
        }

        public static float Distance(Vector2 a, Vector2 b)
        {
            Vector2 vector2 = new Vector2(a.x - b.x, a.y - b.y);
            return MathEx.Sqrt(vector2.x * vector2.x + vector2.y * vector2.y);
        }

        public static float Magnitude(Vector2 v)
        {
            return MathEx.Sqrt(v.x * v.x + v.y * v.y);
        }

        public static float SqrMagnitude(Vector2 v)
        {
            return (v.x * v.x + v.y * v.y);
        }

        public void Scale(Vector2 scale)
        {
            x *= scale.x;
            y *= scale.y;
        }

        public void Normalize()
        {
            float num = Magnitude(this);
            if (num > 9.999999E-06f)
                this /= num;
            else
                this = zero;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2;
        }

        public override bool Equals(object other)
        {
            if (!(other is Vector2))
                return false;

            Vector2 vec = (Vector2)other;
            if (x.Equals(vec.x) && y.Equals(vec.y))
                return true;
            
            return false;
        }
    }
}
