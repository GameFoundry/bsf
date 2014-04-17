using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIArea : ScriptObject
    {
        private GUILayout _layout;
        private GUIPanel parent;
        private bool isDestroyed;

        public GUILayout layout
        {
            get { return _layout;}
        }

        internal GUIArea()
        { }

        internal void SetParent(GUIPanel parent)
        {
            if (this.parent != null)
                this.parent.childAreas.Remove(this);

            this.parent = parent;

            parent.childAreas.Add(this);
        }

        internal static GUIArea Create(GUIPanel parent, int x, int y, int width, int height, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstance(newArea, parent, x, y, width, height, depth);
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableX(GUIPanel parent, int offsetLeft, int offsetRight, int offsetTop, int height, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableX(newArea, parent, offsetLeft, offsetRight, offsetTop, height, depth);
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableY(GUIPanel parent, int offsetTop, int offsetBottom, int offsetLeft, int width, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableY(newArea, parent, offsetTop, offsetBottom, offsetLeft, width, depth);
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        internal static GUIArea CreateResizableXY(GUIPanel parent, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, short depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstanceResizableXY(newArea, parent, offsetLeft, offsetRight, offsetTop, offsetBottom, depth);
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        public void SetArea(int x, int y, int width, int height, short depth = 0)
        {
            // TODO
        }

        public void SetVisible(bool visible)
        {
            Internal_SetVisible(mCachedPtr, visible);
        }

        public bool IsDestroyed()
        {
            return isDestroyed;
        }

        public void Destroy()
        {
            SetParent(null);
            _layout.Destroy();
            isDestroyed = true;

            Internal_Destroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIArea instance, GUIPanel parent, int x, int y, int width, int height, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableX(GUIArea instance, GUIPanel parent, int offsetLeft, int offsetRight, int offsetTop, 
            int height, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableY(GUIArea instance, GUIPanel parent, int offsetTop, int offsetBottom, int offsetLeft,
            int width, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceResizableXY(GUIArea instance, GUIPanel parent, int offsetLeft, int offsetRight, int offsetTop,
            int offsetBottom, short depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);
    }
}
