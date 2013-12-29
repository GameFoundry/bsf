using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUI : GUIBase
    {
        private static GUI instance = new GUI();

        internal GUI()
        {
            Internal_CreateInstance(this);
        }

        public new static GUIArea AddArea(int x, int y, int width = 0, int height = 0, short depth = 0)
        {
            return ((GUIBase)instance).AddArea(x, y, width, height, depth);
        }

        public GUIArea AddResizableAreaX(int offsetLeft, int offsetRight, int offsetTop, int height, short depth = 0)
        {
            return ((GUIBase)instance).AddResizableAreaX(offsetLeft, offsetRight, offsetTop, height, depth);
        }

        public GUIArea AddResizableAreaY(int offsetTop, int offsetBottom, int offsetLeft, int width, short depth = 0)
        {
            return ((GUIBase)instance).AddResizableAreaY(offsetTop, offsetBottom, offsetLeft, width, depth);
        }

        public GUIArea AddResizableAreaXY(int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, short depth = 0)
        {
            return ((GUIBase)instance).AddResizableAreaXY(offsetLeft, offsetRight, offsetTop, offsetBottom, depth);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUI instance);
    }
}
