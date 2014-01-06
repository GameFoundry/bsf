using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GUIElement : ScriptObject
    {
        protected GUIElement parent;
        protected List<GUIElement> children = new List<GUIElement>();

        internal GUIElement(GUIElement parent)
        {
            this.parent = parent;

            if (parent != null)
                parent.children.Add(this);
        }

        public void Destroy()
        {
            for (int i = 0; i < children.Count; i++)
                children[i].Destroy();

            children.Clear();

            if (parent != null)
                parent.children.Remove(this);

            parent = null;

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
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Enable(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Disable(IntPtr nativeInstance);
    }
}
