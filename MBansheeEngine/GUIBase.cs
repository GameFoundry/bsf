using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GUIBase : ScriptObject
    {
        private GUIArea mainArea;
        private GUILayout _mainLayout;

        public GUILayout main
        {
            get { return _mainLayout; }
        }

        internal GUIBase()
        {
            Internal_CreateInstance(this);

            mainArea = AddResizableArea(0, 0, 0, 0, GUIArea.ResizeAxis.XY);
            _mainLayout = mainArea.layout;
        }

        public GUIArea AddArea(int x, int y, int width = 0, int height = 0, short depth = 0)
        {
            return new GUIArea(this, x, y, width, height, depth);
        }

        public GUIArea AddResizableArea(int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, GUIArea.ResizeAxis resizeAxis, short depth = 0)
        {
            return new GUIArea(this, offsetLeft, offsetRight, offsetTop, offsetBottom, resizeAxis, depth);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIBase instance);
    }
}
