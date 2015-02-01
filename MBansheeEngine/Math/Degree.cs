using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public struct Degree
	{
        readonly float value;

		public Degree(float value = 0.0f)
	    {
	        this.value = value;
	    }

        public Degree(Radian r)
        {
            this.value = r.Degrees;
        }

        public static implicit operator Degree(float value)
        {
            return new Degree(value);
        }

        public static implicit operator Degree(Radian r)
        {
            return new Degree(r.Degrees);
        }

	    public static explicit operator float(Degree d)
	    {
            return d.value;
	    }

        public float Degrees
        {
            get { return value; }
        }

        public float Radians
        {
            get { return value*MathEx.Deg2Rad; }
        }

        public static Degree operator+(Degree a)
	    {
	        return a;
	    }

	    public static Degree operator+(Degree a, Degree b)
	    {
	        return new Degree(a.value + b.value);
	    }

        public static Degree operator+(Degree a, Radian r) 
        { 
            return new Degree (a.value + r.Degrees); 
        }

	    public static Degree operator-(Degree a)
	    {
	        return new Degree(-a.value);
	    }

	    public static Degree operator-(Degree a, Degree b)
	    {
	        return new Degree(a.value - b.value);
	    }

        public static Degree operator-(Degree a, Radian r) 
        { 
            return new Degree (a.value - r.Degrees); 
        }

        public static Degree operator*(Degree a, float s)
        {
            return new Degree(a.value * s);
        }

        public static Degree operator*(Degree a, Degree b)
        {
            return new Degree(a.value * b.value);
        }

        public static Degree operator/(Degree a, float s)
        {
            return new Degree(a.value / s);
        }

        public static Degree operator /(Degree a, Degree b)
        {
            return new Degree(a.value / b.value);
        }

	    public static bool operator<(Degree a, Degree b)
	    {
	        return a.value < b.value;
	    }

        public static bool operator>(Degree a, Degree b)
        {
            return a.value > b.value;
        }

        public static bool operator<=(Degree a, Degree b)
        {
            return a.value <= b.value;
        }

        public static bool operator>=(Degree a, Degree b)
        {
            return a.value >= b.value;
        }

        public static bool operator==(Degree a, Degree b)
        {
            return a.value == b.value;
        }

        public static bool operator!=(Degree a, Degree b)
        {
            return a.value != b.value;
        }

        public override bool Equals(object other)
        {
            if (!(other is Degree))
                return false;

            Degree degree = (Degree)other;
            if (value.Equals(degree.value))
                return true;

            return false;
        }

        public override int GetHashCode()
        {
            return value.GetHashCode();
        }

        public override string ToString()
        {
            return value.ToString();
        }
	};
}
