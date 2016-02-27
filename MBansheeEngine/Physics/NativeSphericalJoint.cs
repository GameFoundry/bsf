//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native SphericalJoint class.
    /// <see cref="SphericalJoint"/>
    /// </summary>
    internal class NativeSphericalJoint : NativeJoint
    {
        public LimitConeRange Limit
        {
            set { Internal_SetLimit(mCachedPtr, value); }
        }

        public bool EnableLimit
        {
            set { Internal_SetEnableLimit(mCachedPtr, value); }
        }

        public NativeSphericalJoint()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeSphericalJoint instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPosition(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpeed(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLimit(IntPtr thisPtr, LimitConeRange limit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableLimit(IntPtr thisPtr, bool enable);
    }
}
