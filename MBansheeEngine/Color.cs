using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Color
    {
        public float r;
        public float g;
        public float b;
        public float a;

        public static Color red
        {
            get
            {
                return new Color(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        public static Color green
        {
            get
            {
                return new Color(0.0f, 1.0f, 0.0f, 1.0f);
            }
        }

        public static Color blue
        {
            get
            {
                return new Color(0.0f, 0.0f, 1.0f, 1.0f);
            }
        }

        public static Color white
        {
            get
            {
                return new Color(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }

        public static Color black
        {
            get
            {
                return new Color(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        public float this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0:
                        return r;
                    case 1:
                        return g;
                    case 2:
                        return b;
                    case 3:
                        return a;
                    default:
                        throw new IndexOutOfRangeException("Invalid Color index.");
                }
            }
            set
            {
                switch (index)
                {
                    case 0:
                        r = value;
                        break;
                    case 1:
                        g = value;
                        break;
                    case 2:
                        b = value;
                        break;
                    case 3:
                        a = value;
                        break;
                    default:
                        throw new IndexOutOfRangeException("Invalid Color index.");
                }
            }
        }

        public Color(float r, float g, float b, float a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public Color(float r, float g, float b)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = 1f;
        }

        public static Color operator+ (Color a, Color b)
        {
            return new Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
        }

        public static Color operator- (Color a, Color b)
        {
            return new Color(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
        }

        public static Color operator* (Color a, Color b)
        {
            return new Color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
        }

        public static Color operator* (Color a, float b)
        {
            return new Color(a.r * b, a.g * b, a.b * b, a.a * b);
        }

        public static Color operator* (float b, Color a)
        {
            return new Color(a.r * b, a.g * b, a.b * b, a.a * b);
        }

        public static Color operator/ (Color a, float b)
        {
            return new Color(a.r / b, a.g / b, a.b / b, a.a / b);
        }

        public static bool operator ==(Color lhs, Color rhs)
        {
            return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
        }

        public static bool operator !=(Color lhs, Color rhs)
        {
            return !(lhs == rhs);
        }

        public override int GetHashCode()
        {
            return r.GetHashCode() ^ g.GetHashCode() << 2 ^ b.GetHashCode() >> 2 ^ a.GetHashCode() >> 1;
        }

        public override bool Equals(object other)
        {
            if (!(other is Color))
                return false;

            Color color = (Color)other;
            if (r.Equals(color.r) && g.Equals(color.g) && b.Equals(color.b) && a.Equals(color.a))
                return true;

            return false;
        }
    }
}
