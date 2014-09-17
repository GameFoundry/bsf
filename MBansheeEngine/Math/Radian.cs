using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public sealed class Radian
    {
		readonly float value;

		public Radian(float value = 0.0f)
	    {
	        this.value = value;
	    }

        public Radian(Degree d)
        {
            this.value = d.GetRadians();
        }

        public static implicit operator Radian(float value)
        {
            return new Radian(value);
        }

        public static implicit operator Radian(Degree d)
        {
            return new Radian(d.GetRadians());
        }

        public static explicit operator float(Radian d)
        {
            return d.value;
        }

	    public float GetDegrees()
	    {
	        return value * MathEx.Rad2Deg;
	    }

        public float GetRadians()
        {
            return value;
        }

        public static Radian operator +(Radian a)
	    {
	        return a;
	    }

        public static Radian operator +(Radian a, Radian b)
	    {
            return new Radian(a.value + b.value);
	    }

        public static Radian operator +(Radian a, Degree d) 
        {
            return new Radian(a.value + d.GetRadians()); 
        }

        public static Radian operator -(Radian a)
	    {
            return new Radian(-a.value);
	    }

        public static Radian operator -(Radian a, Radian b)
	    {
            return new Radian(a.value - b.value);
	    }

        public static Radian operator -(Radian a, Degree d) 
        {
            return new Radian(a.value - d.GetRadians()); 
        }

        public static Radian operator *(Radian a, float s)
        {
            return new Radian(a.value * s);
        }

        public static Radian operator *(Radian a, Radian b)
        {
            return new Radian(a.value * b.value);
        }

        public static Radian operator /(Radian a, float s)
        {
            return new Radian(a.value / s);
        }

        public static Radian operator /(Radian a, Radian b)
        {
            return new Radian(a.value / b.value);
        }

        public static bool operator <(Radian a, Radian b)
	    {
	        return a.value < b.value;
	    }

        public static bool operator >(Radian a, Radian b)
        {
            return a.value > b.value;
        }

        public static bool operator <=(Radian a, Radian b)
        {
            return a.value <= b.value;
        }

        public static bool operator >=(Radian a, Radian b)
        {
            return a.value >= b.value;
        }

        public static bool operator ==(Radian a, Radian b)
        {
            return a.value == b.value;
        }

        public static bool operator !=(Radian a, Radian b)
        {
            return a.value != b.value;
        }

        public override int GetHashCode()
        {
            return value.GetHashCode();
        }
    }
}
