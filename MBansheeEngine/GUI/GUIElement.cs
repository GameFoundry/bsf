using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUIElement : ScriptObject
    {
        protected GUILayout parent;
        private bool isDestroyed;

        public Rect2I Bounds
        {
            get { return Internal_GetBounds(mCachedPtr); }
            set { Internal_SetBounds(mCachedPtr, value); }
        }

        public Rect2I VisualBounds
        {
            get { return Internal_GetVisualBounds(mCachedPtr); }
        }

        internal virtual void SetParent(GUILayout layout)
        {
            if (parent != null)
                parent.RemoveInternal(this);

            parent = layout;
        }

        internal virtual bool IsStatic()
        {
            return false;
        }

        public virtual void Destroy()
        {
            SetParent(null);

            Internal_Destroy(mCachedPtr);
            isDestroyed = true;
        }

        public bool IsDestroyed()
        {
            return isDestroyed;
        }

        public void SetVisible(bool visible)
        {
            Internal_SetVisible(mCachedPtr, visible);
        }

        public void SetPosition(int x, int y)
        {
            Internal_SetPosition(mCachedPtr, x, y);
        }

        public void SetWidth(int width)
        {
            Internal_SetWidth(mCachedPtr, width);
        }

        public void SetFlexibleWidth(int minWidth, int maxWidth)
        {
            Internal_SetFlexibleWidth(mCachedPtr, minWidth, maxWidth);
        }

        public void SetHeight(int height)
        {
            Internal_SetHeight(mCachedPtr, height);
        }

        public void SetFlexibleHeight(int minHeight, int maxHeight)
        {
            Internal_SetFlexibleHeight(mCachedPtr, minHeight, maxHeight);
        }

        public void ResetDimensions()
        {
            Internal_ResetDimensions(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr nativeInstance, int x, int y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetWidth(IntPtr nativeInstance, int width);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFlexibleWidth(IntPtr nativeInstance, int minWidth, int maxWidth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHeight(IntPtr nativeInstance, int height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFlexibleHeight(IntPtr nativeInstance, int minHeight, int maxHeight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ResetDimensions(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2I Internal_GetBounds(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBounds(IntPtr nativeInstance, Rect2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2I Internal_GetVisualBounds(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);
    }
}
