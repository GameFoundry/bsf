using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUIElement : ScriptObject
    {
        public Action<bool> OnFocusChanged;

        public Rect2I Bounds
        {
            get { return Internal_GetBounds(mCachedPtr); }
            set { Internal_SetBounds(mCachedPtr, value); }
        }

        public Rect2I VisualBounds
        {
            get { return Internal_GetVisualBounds(mCachedPtr); }
        }

        private void InternalOnFocusChanged(bool focus)
        {
            if (OnFocusChanged != null)
                OnFocusChanged(focus);
        }

        public virtual void Destroy()
        {
            Internal_Destroy(mCachedPtr);
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

        public void SetContextMenu(ContextMenu menu)
        {
            IntPtr menuPtr = IntPtr.Zero;
            if (menu != null)
                menuPtr = menu.GetCachedPtr();

            Internal_SetContextMenu(mCachedPtr, menuPtr);
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
        private static extern void Internal_SetContextMenu(IntPtr nativeInstance, IntPtr contextMenu);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);
    }
}
