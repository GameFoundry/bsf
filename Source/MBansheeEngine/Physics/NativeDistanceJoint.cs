//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native DistanceJoint class.
    /// <see cref="DistanceJoint"/>
    /// </summary>
    internal class NativeDistanceJoint : NativeJoint
    {
        public float Distance
        {
            get { return Internal_GetDistance(mCachedPtr); }
        }

        public float MinDistance
        {
            set { Internal_SetMinDistance(mCachedPtr, value); }
        }

        public float MaxDistance
        {
            set { Internal_SetMaxDistance(mCachedPtr, value); }
        }

        public float Tolerance
        {
            set { Internal_SetTolerance(mCachedPtr, value); }
        }

        public Spring Spring
        {
            set { Internal_SetSpring(mCachedPtr, ref value); }
        }

        public bool EnableMinDistanceLimit
        {
            set { Internal_SetEnableMinDistanceLimit(mCachedPtr, value); }
        }

        public bool EnableMaxDistanceLimit
        {
            set { Internal_SetEnableMaxDistanceLimit(mCachedPtr, value); }
        }

        public bool EnableSpring
        {
            set { Internal_SetEnableSpring(mCachedPtr, value); }
        }

        public NativeDistanceJoint(ScriptCommonJointData commonData, ScriptDistanceJointData data)
        {
            Internal_CreateInstance(this, ref commonData, ref data);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeDistanceJoint instance, ref ScriptCommonJointData commonData,
           ref ScriptDistanceJointData data);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetDistance(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMinDistance(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaxDistance(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTolerance(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSpring(IntPtr thisPtr, ref Spring value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableMinDistanceLimit(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableMaxDistanceLimit(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableSpring(IntPtr thisPtr, bool value);
    }

    /// <summary>
    /// Used for passing DistanceJoint initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptDistanceJointData // Note: Must match C++ struct ScriptDistanceJointData
    {
        public float minDistance;
        public float maxDistance;
        public float tolerance;
        public Spring spring;
        public DistanceJoint.Flag flags;
    }
}
