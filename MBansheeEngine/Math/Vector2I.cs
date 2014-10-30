using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2I
    {
        public int x;
        public int y;

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
                    default:
                        throw new IndexOutOfRangeException("Invalid Vector2I index.");
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
                        throw new IndexOutOfRangeException("Invalid Vector2I index.");
                }
            }
        }

        public Vector2I(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public static Vector2I operator +(Vector2I a, Vector2I b)
        {
            return new Vector2I(a.x + b.x, a.y + b.y);
        }

        public static Vector2I operator -(Vector2I a, Vector2I b)
        {
            return new Vector2I(a.x - b.x, a.y - b.y);
        }

        public static Vector2I operator -(Vector2I v)
        {
            return new Vector2I(-v.x, -v.y);
        }

        public static Vector2I operator *(Vector2I v, int d)
        {
            return new Vector2I(v.x * d, v.y * d);
        }

        public static Vector2I operator *(int d, Vector2I v)
        {
            return new Vector2I(v.x * d, v.y * d);
        }

        public static Vector2I operator /(Vector2I v, int d)
        {
            return new Vector2I(v.x / d, v.y / d);
        }

        public static bool operator ==(Vector2I lhs, Vector2I rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }

        public static bool operator !=(Vector2I lhs, Vector2I rhs)
        {
            return !(lhs == rhs);
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() ^ y.GetHashCode() << 2;
        }

        public override bool Equals(object other)
        {
            if (!(other is Vector2I))
                return false;

            Vector2I vec = (Vector2I)other;
            if (x.Equals(vec.x) && y.Equals(vec.y))
                return true;

            return false;
        }

        public override string ToString()
        {
            return "(" + x + ", " + y + ")";
        }
    }
}
