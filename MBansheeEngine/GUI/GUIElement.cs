using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUIElement : ScriptObject
    {
        protected GUILayout parent;
        private bool isDestroyed;

        internal virtual void SetParent(GUILayout layout)
        {
            if (parent != null)
                parent.Remove(this);

            parent = layout;

            if (parent != null)
                parent.children.Add(this);

            Internal_SetParent(mCachedPtr, layout);
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
        private static extern void Internal_SetParent(IntPtr nativeInstance, GUILayout parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayoutOptions(IntPtr nativeInstance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);
    }
}
