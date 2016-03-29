//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native HingeJoint class.
    /// <see cref="HingeJoint"/>
    /// </summary>
    internal class NativeHingeJoint : NativeJoint
    {
        public Radian Angle
        {
            get { return new Radian(Internal_GetAngle(mCachedPtr)); }
        }

        public float Speed
        {
            get { return Internal_GetSpeed(mCachedPtr); }
        }

        public LimitAngularRange Limit
        {
            set { Internal_SetLimit(mCachedPtr, value); }
        }

        public HingeJointDrive Drive
        {
            set { Internal_SetDrive(mCachedPtr, value); }
        }

        public bool EnableLimit
        {
            set { Internal_SetEnableLimit(mCachedPtr, value); }
        }

        public bool EnableDrive
        {
            set { Internal_SetEnableDrive(mCachedPtr, value); }
        }

        public NativeHingeJoint(ScriptCommonJointData commonData, ScriptHingeJointData data)
        {
            Internal_CreateInstance(this, ref commonData, ref data);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeHingeJoint instance, ref ScriptCommonJointData commonData,
            ref ScriptHingeJointData data);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetAngle(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpeed(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLimit(IntPtr thisPtr, LimitAngularRange limit);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDrive(IntPtr thisPtr, HingeJointDrive drive);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableLimit(IntPtr thisPtr, bool enable);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableDrive(IntPtr thisPtr, bool enable);
    }

    /// <summary>
    /// Used for passing HingeJoint initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptHingeJointData // Note: Must match C++ struct ScriptHingeJointData
    {
        public LimitAngularRange limit;
        public HingeJointDrive drive;
        public HingeJoint.Flag flags;
    }
}
