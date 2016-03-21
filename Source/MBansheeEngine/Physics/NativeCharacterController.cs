//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native CharacterController class.
    /// <see cref="CharacterController"/>
    /// </summary>
    internal class NativeCharacterController : ScriptObject
    {
        private CharacterController component;

        /// <summary>
        /// Component that owns the native character controller object.
        /// </summary>
        public CharacterController Component
        {
            get { return component; }
            set { component = value; }
        }

        public Vector3 Position
        {
            get { Vector3 pos; Internal_GetPosition(mCachedPtr, out pos); return pos; }
            set { Internal_SetPosition(mCachedPtr, ref value); }
        }

        public Vector3 FootPosition
        {
            get { Vector3 pos; Internal_GetFootPosition(mCachedPtr, out pos); return pos; }
            set { Internal_SetFootPosition(mCachedPtr, ref value); }
        }

        public float Radius
        {
            set { Internal_SetRadius(mCachedPtr, value); }
        }

        public float Height
        {
            set { Internal_SetHeight(mCachedPtr, value); }
        }

        public Vector3 Up
        {
            set { Internal_SetUp(mCachedPtr, ref value); }
        }

        public CharacterClimbingMode ClimbingMode
        {
            set { Internal_SetClimbingMode(mCachedPtr, value); }
        }

        public CharacterNonWalkableMode NonWalkableMode
        {
            set { Internal_SetNonWalkableMode(mCachedPtr, value); }
        }

        public float MinMoveDistance
        {
            set { Internal_SetMinMoveDistance(mCachedPtr, value); }
        }

        public float ContactOffset
        {
            set { Internal_SetContactOffset(mCachedPtr, value); }
        }

        public float StepOffset
        {
            set { Internal_SetStepOffset(mCachedPtr, value); }
        }

        public Radian SlopeLimit
        {
            set { Internal_SetSlopeLimit(mCachedPtr, value.Radians); }
        }

        public ulong Layer
        {
            set { Internal_SetLayer(mCachedPtr, value); }
        }

        public NativeCharacterController(ScriptCharacterControllerData initData)
        {
            Internal_CreateInstance(this, ref initData);
        }

        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        public void Move(Vector3 position)
        {
            Internal_Move(mCachedPtr, ref position);
        }

        private void Internal_DoOnColliderHit(ScriptControllerCollision scriptCollisionData)
        {
            ControllerColliderCollision collisionData = new ControllerColliderCollision();
            collisionData.position = scriptCollisionData.position;
            collisionData.normal = scriptCollisionData.normal;
            collisionData.motionDir = scriptCollisionData.motionDir;
            collisionData.motionAmount = scriptCollisionData.motionAmount;
            collisionData.triangleIndex = scriptCollisionData.triangleIndex;

            if(scriptCollisionData.collider != null)
                collisionData.collider = scriptCollisionData.collider.Component;
            else
                collisionData.collider = null;

            Component.DoOnColliderHit(collisionData);
        }

        private void Internal_DoOnControllerHit(ScriptControllerCollision scriptCollisionData)
        {
            ControllerControllerCollision collisionData = new ControllerControllerCollision();
            collisionData.position = scriptCollisionData.position;
            collisionData.normal = scriptCollisionData.normal;
            collisionData.motionDir = scriptCollisionData.motionDir;
            collisionData.motionAmount = scriptCollisionData.motionAmount;

            if (scriptCollisionData.controller != null)
                collisionData.controller = scriptCollisionData.controller.Component;
            else
                collisionData.controller = null;

            Component.DoOnControllerHit(collisionData);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeCharacterController instance, 
            ref ScriptCharacterControllerData initData);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Move(IntPtr thisPtr, ref Vector3 position);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPosition(IntPtr thisPtr, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr thisPtr, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetFootPosition(IntPtr thisPtr, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFootPosition(IntPtr thisPtr, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRadius(IntPtr thisPtr, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHeight(IntPtr thisPtr, float height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetUp(IntPtr thisPtr, ref Vector3 up);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetClimbingMode(IntPtr thisPtr, CharacterClimbingMode mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetNonWalkableMode(IntPtr thisPtr, CharacterNonWalkableMode mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMinMoveDistance(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContactOffset(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetStepOffset(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSlopeLimit(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayer(IntPtr thisPtr, ulong layer);
    }
}
