using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIFixedSpace : GUIElement
    {
        internal GUIFixedSpace(GUILayout parentLayout, int size)
        {
            Internal_CreateInstance(this, parentLayout, size);
        }

        internal override bool IsStatic()
        {
            return true;
        }

        internal override void SetParent(GUILayout layout)
        {
            // Space only gets one parent set on initialization and then it cannot be moved
            if (parent == null)
            {
                parent = layout;

                if (parent != null)
                    parent.children.Add(this);
            }
        }

        public void SetSize(int size)
        {
            Internal_SetSize(mCachedPtr, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFixedSpace instance, GUILayout parentLayout, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSize(IntPtr nativeInstance, int size);
    }

    public sealed class GUIFlexibleSpace : GUIElement
    {
        internal GUIFlexibleSpace(GUILayout parentLayout)
        {
            Internal_CreateInstance(this, parentLayout);
        }

        internal override bool IsStatic()
        {
            return true;
        }

        internal override void SetParent(GUILayout layout)
        {
            // Space only gets one parent set on initialization and then it cannot be moved
            if (parent == null)
            {
                parent = layout;

                if (parent != null)
                    parent.children.Add(this);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFlexibleSpace instance, GUILayout parentLayout);
    }
}
