using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GUIElement : ScriptObject
    {
        public void Destroy()
        {
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
