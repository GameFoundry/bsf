using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class HString : ScriptObject
    {
        public HString(string identifier)
        {
            Internal_CreateInstance(this, identifier);
        }

        public void setParameter(int idx, string value)
        {
            Internal_SetParameter(mCachedPtr, idx, value);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HString instance, string identifier);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetParameter(IntPtr nativeInstance, int idx, string identifier);
    }
}
