using System;

namespace BansheeEngine
{
    public enum EulerAngleOrder
	{
		XYZ,
		XZY,
		YXZ,
		YZX,
		ZXY,
		ZYX
	};

    class MathEx
    {
        public const float Pi = 3.141593f;
        public const float TwoPi = (2.0f * Pi);
        public const float HalfPi = (0.5f * Pi);
        public const float Deg2Rad = Pi / 180.0f;
        public const float Rad2Deg = 180.0f / Pi;

        public static float Min(float a, float b)
        {
            if (a < b)
                return a;
            
            return b;
        }

        public static float Min(params float[] values)
        {
            int length = values.Length;
            if (length == 0)
                return 0.0f;

            float min = values[0];
            for (int i = 1; i < length; i++)
            {
                if (values[i] < min)
                    min = values[i];
            }

            return min;
        }

        public static int Min(int a, int b)
        {
            if (a < b)
                return a;

            return b;
        }

        public static int Min(params int[] values)
        {
            int length = values.Length;
            if (length == 0)
                return 0;

            int min = values[0];
            for (int i = 1; i < length; i++)
            {
                if (values[i] < min)
                    min = values[i];
            }

            return min;
        }

        public static float Max(float a, float b)
        {
            if (a > b)
                return a;

            return b;
        }

        public static float Max(params float[] values)
        {
            int length = values.Length;
            if (length == 0)
                return 0.0f;

            float max = values[0];
            for (int i = 1; i < length; i++)
            {
                if (values[i] > max)
                    max = values[i];
            }

            return max;
        }

        public static int Max(int a, int b)
        {
            if (a > b)
                return a;
            else
                return b;
        }

        public static int Max(params int[] values)
        {
            int length = values.Length;
            if (length == 0)
                return 0;

            int max = values[0];
            for (int i = 1; i < length; ++i)
            {
                if (values[i] > max)
                    max = values[i];
            }

            return max;
        }

        public static float Abs(float f)
        {
            return Math.Abs(f);
        }

        public static int Abs(int value)
        {
            return Math.Abs(value);
        }

        public static float Pow(float f, float p)
        {
            return (float)Math.Pow(f, p);
        }

        public static float Exp(float power)
        {
            return (float)Math.Exp(power);
        }

        public static float Log(float f, float p)
        {
            return (float)Math.Log(f, p);
        }

        public static float Log(float f)
        {
            return (float)Math.Log(f);
        }

        public static float Log10(float f)
        {
            return (float)Math.Log10(f);
        }

        public static float Ceil(float f)
        {
            return (float)Math.Ceiling(f);
        }

        public static float Floor(float f)
        {
            return (float)Math.Floor(f);
        }

        public static float Round(float f)
        {
            return (float)Math.Round(f);
        }

        public static int CeilToInt(float f)
        {
            return (int)Math.Ceiling(f);
        }

        public static int FloorToInt(float f)
        {
            return (int)Math.Floor(f);
        }

        public static int RoundToInt(float f)
        {
            return (int)Math.Round(f);
        }

        public static float Sign(float f)
        {
            return f >= 0.0f ? 1.0f : -1.0f;
        }

        public static float Sin(float f)
        {
            return (float)Math.Sin(f);
        }

        public static float Cos(float f)
        {
            return (float)Math.Cos(f);
        }

        public static float Tan(float f)
        {
            return (float)Math.Tan(f);
        }

        public static float Asin(float f)
        {
            return (float)Math.Asin(f);
        }

        public static float Acos(float f)
        {
            return (float)Math.Acos(f);
        }

        public static float Atan(float f)
        {
            return (float)Math.Atan(f);
        }

        public static float Atan2(float y, float x)
        {
            return (float)Math.Atan2(y, x);
        }

        public static float Sqrt(float f)
        {
            return (float)Math.Sqrt(f);
        }

        public static float InvSqrt(float f)
        {
            return 1.0f/(float) Math.Sqrt(f);
        }

        public static float Clamp(float value, float min, float max)
        {
            if (value < min)
                value = min;
            else if (value > max)
                value = max;

            return value;
        }

        public static int Clamp(int value, int min, int max)
        {
            if (value < min)
                value = min;
            else if (value > max)
                value = max;

            return value;
        }

        public static float Clamp01(float value)
        {
            if (value < 0.0)
                return 0.0f;

            if (value > 1.0)
                return 1f;
            
            return value;
        }
    }
}
