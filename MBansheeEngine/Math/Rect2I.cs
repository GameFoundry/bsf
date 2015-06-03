using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Rect2I
    {
        public Rect2I(int x, int y, int width, int height)
        {
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
        }

		public static bool operator== (Rect2I lhs, Rect2I rhs)
		{
			return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height;
		}

		public static bool operator!= (Rect2I lhs, Rect2I rhs)
		{
			return !(lhs == rhs);
		}

	    public bool Contains(Vector2I point)
	    {
		    if(point.x >= x && point.x < (x + width))
		    {
			    if(point.y >= y && point.y < (y + height))
				    return true;
		    }

		    return false;
	    }

	    public bool Overlaps(Rect2I other)
	    {
		    int otherRight = other.x + other.width;
            int myRight = x + width;

            int otherBottom = other.y + other.height;
            int myBottom = y + height;

		    if(x < otherRight && myRight > other.x &&
			    y < otherBottom && myBottom > other.y)
			    return true;

		    return false;
	    }

	    public void Clip(Rect2I clipRect)
	    {
		    int newLeft = Math.Max(x, clipRect.x);
		    int newTop = Math.Max(y, clipRect.y);

		    int newRight = Math.Min(x + width, clipRect.x + clipRect.width);
		    int newBottom = Math.Min(y + height, clipRect.y + clipRect.height);

		    x = Math.Min(newLeft, newRight);
		    y = Math.Min(newTop, newBottom);
		    width = Math.Max(0, newRight - newLeft);
            height = Math.Max(0, newBottom - newTop);
	    }

        public override bool Equals(object other)
        {
            if (!(other is Rect2I))
                return false;

            Rect2I rect = (Rect2I)other;
            if (x.Equals(rect.x) && y.Equals(rect.y) && width.Equals(rect.width) && height.Equals(rect.height))
                return true;

            return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override string ToString()
        {
            return String.Format("(x:{0} y:{1} width:{2} height:{3})", x, y, width, height);
        }

        public int x, y, width, height;
    }
}
