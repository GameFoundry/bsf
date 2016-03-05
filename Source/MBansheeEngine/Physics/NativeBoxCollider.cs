//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native BoxCollider class.
    /// <see cref="BoxCollider"/>
    /// </summary>
    internal class NativeBoxCollider : NativeCollider
    {
        public NativeBoxCollider()
        {
            Internal_CreateInstance(this);
        }

        public Vector3 Extents
        {
            get { Vector3 extents; Internal_GetExtents(mCachedPtr, out extents); return extents; }
            set { Internal_SetExtents(mCachedPtr, ref value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeBoxCollider instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetExtents(IntPtr thisPtr, ref Vector3 extents);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetExtents(IntPtr thisPtr, out Vector3 extents);
    }
}
