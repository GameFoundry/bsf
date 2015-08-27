using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class ScriptObject
    {
        internal IntPtr mCachedPtr;

        ~ScriptObject()
        {
            if (mCachedPtr == IntPtr.Zero)
            {
                Debug.LogError("Script object is being finalized but doesn't have a pointer to its interop object. Type: " + GetType());

#if DEBUG
                // This will cause a crash, so we ignore it in release mode hoping all it causes is a memory leak.
                Internal_ManagedInstanceDeleted(mCachedPtr);
#endif
            }
            else
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
