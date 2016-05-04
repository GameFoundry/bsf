//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @cond INTEROP */
    /** @addtogroup Interop
     *  @{
     */

    /// <summary>
    /// Wrapper around the native CapsuleCollider class.
    /// <see cref="CapsuleCollider"/>
    /// </summary>
    internal class NativeCapsuleCollider : NativeCollider
    {
        public NativeCapsuleCollider()
        {
            Internal_CreateInstance(this);
        }

        public float Radius
        {
            get { return Internal_GetRadius(mCachedPtr); }
            set { Internal_SetRadius(mCachedPtr, value); }
        }

        public float HalfHeight
        {
            get { return Internal_GetHalfHeight(mCachedPtr); }
            set { Internal_SetHalfHeight(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeCapsuleCollider instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetHalfHeight(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHalfHeight(IntPtr thisPtr, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRadius(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRadius(IntPtr thisPtr, float radius);
    }

    /** @} */
    /** @endcond */
}
