//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @cond INTEROP */

    /// <summary>
    /// A base class for all script objects that interface with the native code.
    /// </summary>
    public class ScriptObject
    {
        /// <summary>
        /// A pointer to the native script interop object.
        /// </summary>
        internal IntPtr mCachedPtr;

        /// <summary>
        /// Notifies the native script interop object that the managed instance was finalized.
        /// </summary>
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

        /// <summary>
        /// Returns a pointer to the native script interop object.
        /// </summary>
        /// <returns>Pointer to the native script interop object</returns>
        internal IntPtr GetCachedPtr()
        {
            return mCachedPtr;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ManagedInstanceDeleted(IntPtr nativeInstance);
    }

    /** @endcond */
}
