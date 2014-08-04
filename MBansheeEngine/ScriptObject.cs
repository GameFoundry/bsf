using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class ScriptObject
    {
        internal IntPtr mCachedPtr;

        ~ScriptObject()
        {
            Internal_ManagedInstanceDeleted(mCachedPtr);
        }

        internal IntPtr GetCachedPtr()
        {
            return mCachedPtr;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ManagedInstanceDeleted(IntPtr nativeInstance);
    }
}
