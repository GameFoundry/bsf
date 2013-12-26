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

        public new static GUIArea AddResizableArea(int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, GUIArea.ResizeAxis resizeAxis, short depth = 0)
        {
            return ((GUIBase)instance).AddResizableArea(offsetLeft, offsetRight, offsetTop, offsetBottom, resizeAxis, depth);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUI instance);
    }
}
