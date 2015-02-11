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

            if(parent != null)
                parent.childAreas.Add(this);
        }

        // Note: Should only ever be called by its parent GUIPanel
        internal static GUIArea Create(GUIPanel parent, int x, int y, int width, int height, UInt16 depth)
        {
            GUIArea newArea = new GUIArea();
            Internal_CreateInstance(newArea, parent, x, y, width, height, depth);
            newArea._layout = new GUILayoutX(newArea);

            return newArea;
        }

        public void SetArea(int x, int y, int width, int height, UInt16 depth = 0)
        {
            Internal_SetArea(mCachedPtr, x, y, width, height, depth);
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
        private static extern void Internal_CreateInstance(GUIArea instance, GUIPanel parent, int x, int y, int width, int height, UInt16 depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetArea(IntPtr nativeInstance, int x, int y, int width, int height, UInt16 depth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);
    }
}
