//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native Joint class.
    /// <see cref="Joint"/>
    /// </summary>
    internal class NativeJoint : ScriptObject
    {
        private Joint component;

        /// <summary>
        /// Component that owns the native joint object.
        /// </summary>
        public Joint Component
        {
            get { return component; }
            set { component = value; }
        }

        public float BreakForce
        {
            set { Internal_SetBreakForce(mCachedPtr, value); }
        }

        public float BreakTorque
        {
            set { Internal_SetBreakTorque(mCachedPtr, value); }
        }

        public bool EnableCollision
        {
            set { Internal_SetEnableCollision(mCachedPtr, value); }
        }

        public void SetRigidbody(JointBody body, Rigidbody rigidbody)
        {
            IntPtr rigidbodyPtr = IntPtr.Zero;
            if (rigidbody != null)
                rigidbodyPtr = rigidbody.native.GetCachedPtr();

            Internal_SetBody(mCachedPtr, body, rigidbodyPtr);
        }

        public void SetPosition(JointBody body, Vector3 position)
        {
            Internal_SetPosition(mCachedPtr, body, ref position);
        }

        public void SetRotation(JointBody body, Quaternion rotation)
        {
            Internal_SetRotation(mCachedPtr, body, ref rotation);
        }
        
        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        private void Internal_DoOnJointBreak()
        {
            Component.DoOnJointBreak();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBody(IntPtr thisPtr, JointBody body, IntPtr rigidbody);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr thisPtr, JointBody body, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotation(IntPtr thisPtr, JointBody body, ref Quaternion rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBreakForce(IntPtr thisPtr, float force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBreakTorque(IntPtr thisPtr, float torque);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEnableCollision(IntPtr thisPtr, bool value);
    }

    /// <summary>
    /// Used for passing common Joint initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptCommonJointData // Note: Must match C++ struct ScriptCommonJointData
    {
        public IntPtr[] bodies;
        public Vector3[] positions;
        public Quaternion[] rotations;
        public float breakForce;
        public float breakTorque;
        public bool enableCollision;
    }
}
