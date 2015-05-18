using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIFixedSpace : GUIElement
    {
        public GUIFixedSpace(int size)
        {
            Internal_CreateInstance(this, size);
        }

        public void SetSize(int size)
        {
            Internal_SetSize(mCachedPtr, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFixedSpace instance, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSize(IntPtr nativeInstance, int size);
    }

    public sealed class GUIFlexibleSpace : GUIElement
    {
        public GUIFlexibleSpace()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIFlexibleSpace instance);
    }
}
