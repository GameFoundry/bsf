using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIArea : ScriptObject
    {
        private GUILayout _layout;

        public GUILayout layout
        {
            get { return _layout;}
        }

        public enum ResizeAxis
        {
            X,
            Y,
            XY
        }

        internal GUIArea(GUIBase parent, int x, int y, int width, int height, short depth)
        {
            Internal_CreateInstance(this, parent, x, y, width, height, depth);
            _layout = new LayoutX(this);
        }

        internal GUIArea(GUIBase parent, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, ResizeAxis resizeAxis, short depth)
        {
            Internal_CreateResizableInstance(this, parent, offsetLeft, offsetRight, offsetTop, offsetBottom, resizeAxis, depth);
            _layout = new LayoutX(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIArea instance, GUIBase parent, int x, int y, int width, int height, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateResizableInstance(GUIArea instance, GUIBase parent, int offsetLeft, int offsetRight, int offsetTop, 
            int offsetBottom, ResizeAxis resizeAxis, short depth);
    }
}
