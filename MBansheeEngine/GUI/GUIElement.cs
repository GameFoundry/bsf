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

        public void SetLayoutOptions(params GUIOption[] options)
        {
            Internal_SetLayoutOptions(mCachedPtr, options);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayoutOptions(IntPtr nativeInstance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);

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
