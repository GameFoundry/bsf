//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native SphereCollider class.
    /// <see cref="SphereCollider"/>
    /// </summary>
    internal class NativeSphereCollider : NativeCollider
    {
        public NativeSphereCollider()
        {
            Internal_CreateInstance(this);
        }

        public float Radius
        {
            get { return Internal_GetRadius(mCachedPtr); }
            set { Internal_SetRadius(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeSphereCollider instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRadius(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRadius(IntPtr thisPtr, float radius);
    }
}
