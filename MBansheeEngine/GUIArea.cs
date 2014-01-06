using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIArea : ScriptObject
    {
        private GUILayout _layout;
        private GUIBase parent;

        public GUILayout layout
        {
            get { return _layout;}
        }

        internal GUIArea()
        { }

        internal static GUIArea Create(GUIBase parent, int x, int y, int width, int height, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstance(newArea, parent, x, y, width, height, depth);
            newArea.parent = parent;
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableX(GUIBase parent, int offsetLeft, int offsetRight, int offsetTop, int height, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableX(newArea, parent, offsetLeft, offsetRight, offsetTop, height, depth);
            newArea.parent = parent;
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableY(GUIBase parent, int offsetTop, int offsetBottom, int offsetLeft, int width, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableY(newArea, parent, offsetTop, offsetBottom, offsetLeft, width, depth);
            newArea.parent = parent;
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableXY(GUIBase parent, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableXY(newArea, parent, offsetLeft, offsetRight, offsetTop, offsetBottom, depth);
            newArea.parent = parent;
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        public void Destroy()
        {
            _layout.Destroy();
            parent.childAreas.Remove(this);

            Internal_Destroy(mCachedPtr);
        }

        public void Enable()
        {
            Internal_Enable(mCachedPtr);
        }

        public void Disable()
        {
            Internal_Disable(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIArea instance, GUIBase parent, int x, int y, int width, int height, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableX(GUIArea instance, GUIBase parent, int offsetLeft, int offsetRight, int offsetTop, 
            int height, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableY(GUIArea instance, GUIBase parent, int offsetTop, int offsetBottom, int offsetLeft,
            int width, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableXY(GUIArea instance, GUIBase parent, int offsetLeft, int offsetRight, int offsetTop,
            int offsetBottom, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Enable(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Disable(IntPtr nativeInstance);
    }
}
