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

        public static Color Red { get { return new Color(1.0f, 0.0f, 0.0f, 1.0f); } }
        public static Color Green { get { return new Color(0.0f, 1.0f, 0.0f, 1.0f); } }
        public static Color Blue { get { return new Color(0.0f, 0.0f, 1.0f, 1.0f); } }
        public static Color Yellow { get { return new Color(1.0f, 1.0f, 0.0f, 1.0f); } }
        public static Color Cyan { get { return new Color(0.0f, 1.0f, 1.0f, 1.0f); } }
        public static Color Magenta { get { return new Color(1.0f, 0.0f, 1.0f, 1.0f); } }
        public static Color White { get { return new Color(1.0f, 1.0f, 1.0f, 1.0f); } }
        public static Color Black { get { return new Color(0.0f, 0.0f, 0.0f, 1.0f); } }

        public static Color BansheeOrange { get { return new Color(1.0f, (168.0f/255.0f), 0.0f, 1.0f); } }

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

        public static Color RGB2HSV(Color input)
        {
            Color output = input;

            float min = input.r < input.g ? input.r : input.g;
            min = min < input.b ? min : input.b;

            float max = input.r > input.g ? input.r : input.g;
            max = max > input.b ? max : input.b;

            output.b = max;

            float delta = max - min;
            if (max > 0.0f)
                output.g = (delta / max);
            else
            {
                output.g = 0.0f;
                output.r = 0.0f;
                return output;
            }

            if (input.r >= max)
                output.r = (input.g - input.b) / delta;
            else
            {
                if (input.g >= max)
                    output.r = 2.0f + (input.b - input.r) / delta;
                else
                    output.r = 4.0f + (input.r - input.g) / delta;
            }

            output.r /= 6.0f;

            if (output.r < 0.0f)
                output.r += 1.0f;

            return output;
        }

        public static Color HSV2RGB(Color input)
        {
            Color output = input;

            if (input.g <= 0.0)
            {
                output.r = input.b;
                output.g = input.b;
                output.b = input.b;

                return output;
            }

            float hh = input.r;
            if (hh >= 1.0f)
                hh = 0.0f;

            hh *= 6.0f;

            int i = (int)hh;
            float ff = hh - i;
            float p = input.b * (1.0f - input.g);
            float q = input.b * (1.0f - (input.g * ff));
            float t = input.b * (1.0f - (input.g * (1.0f - ff)));

            switch (i)
            {
                case 0:
                    output.r = input.b;
                    output.g = t;
                    output.b = p;
                    break;
                case 1:
                    output.r = q;
                    output.g = input.b;
                    output.b = p;
                    break;
                case 2:
                    output.r = p;
                    output.g = input.b;
                    output.b = t;
                    break;
                case 3:
                    output.r = p;
                    output.g = q;
                    output.b = input.b;
                    break;
                case 4:
                    output.r = t;
                    output.g = p;
                    output.b = input.b;
                    break;
                default:
                    output.r = input.b;
                    output.g = p;
                    output.b = q;
                    break;
            }

            return output;
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
