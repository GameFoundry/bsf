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

        public GUISkin skin; // TODO

        internal GUIBase()
        {
            Internal_CreateInstance(this);

            mainArea = AddResizableAreaXY(0, 0, 0, 0);
            _mainLayout = mainArea.layout;
        }

        public GUIArea AddArea(int x, int y, int width = 0, int height = 0, short depth = 0)
        {
            return GUIArea.Create(this, x, y, width, height, depth);
        }

        public GUIArea AddResizableAreaX(int offsetLeft, int offsetRight, int offsetTop, int height, short depth = 0)
        {
            return GUIArea.CreateResizableX(this, offsetLeft, offsetRight, offsetTop, height, depth);
        }

        public GUIArea AddResizableAreaY(int offsetTop, int offsetBottom, int offsetLeft, int width, short depth = 0)
        {
            return GUIArea.CreateResizableY(this, offsetTop, offsetBottom, offsetLeft, width, depth);
        }

        public GUIArea AddResizableAreaXY(int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, short depth = 0)
        {
            return GUIArea.CreateResizableXY(this, offsetLeft, offsetRight, offsetTop, offsetBottom, depth);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIBase instance);
    }
}
