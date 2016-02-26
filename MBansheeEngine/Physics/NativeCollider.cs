//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native Collider class.
    /// <see cref="Collider"/>
    /// </summary>
    internal class NativeCollider : ScriptObject
    {
        private Collider component;

        /// <summary>
        /// Component that owns the native collider object.
        /// </summary>
        public Collider Component
        {
            get { return component; }
            set { component = value; }
        }

        public Vector3 Position
        {
            get { Vector3 pos; Internal_GetPosition(mCachedPtr, out pos); return pos; }
            set { Quaternion rot = Rotation; Internal_SetTransform(mCachedPtr, ref value, ref rot); }
        }

        public Quaternion Rotation
        {
            get { Quaternion rot; Internal_GetRotation(mCachedPtr, out rot); return rot; }
            set { Vector3 pos = Position; Internal_SetTransform(mCachedPtr, ref pos, ref value); }
        }

        public Vector3 Scale
        {
            get { Vector3 scale; Internal_GetScale(mCachedPtr, out scale); return scale; }
            set { Internal_SetScale(mCachedPtr, ref value); }
        }

        public bool IsTrigger
        {
            get { return Internal_GetIsTrigger(mCachedPtr); }
            set { Internal_SetIsTrigger(mCachedPtr, value); }
        }

        public NativeRigidbody Rigidbody
        {
            get { return Internal_GetRigidbody(mCachedPtr); }
            set
            {
                IntPtr rigidbodyPtr = IntPtr.Zero;
                if (value != null)
                    rigidbodyPtr = value.GetCachedPtr();

                Internal_SetRigidbody(mCachedPtr, rigidbodyPtr);
            }
        }

        public float Mass
        {
            get { return Internal_GetMass(mCachedPtr); }
            set { Internal_SetMass(mCachedPtr, value); }
        }

        public PhysicsMaterial Material
        {
            get { return Internal_GetMaterial(mCachedPtr); }
            set
            {
                IntPtr materialPtr = IntPtr.Zero;
                if (value != null)
                    materialPtr = value.GetCachedPtr();

                Internal_SetMaterial(mCachedPtr, materialPtr);
            }
        }

        public float ContactOffset
        {
            get { return Internal_GetContactOffset(mCachedPtr); }
            set { Internal_SetContactOffset(mCachedPtr, value); }
        }

        public float RestOffset
        {
            get { return Internal_GetRestOffset(mCachedPtr); }
            set { Internal_SetRestOffset(mCachedPtr, value); }
        }

        public ulong Layer
        {
            get { return Internal_GetLayer(mCachedPtr); }
            set { Internal_SetLayer(mCachedPtr, value); }
        }

        public CollisionReportMode CollisionReportMode
        {
            get { return Internal_GetCollisionReportMode(mCachedPtr); }
            set { Internal_SetCollisionReportMode(mCachedPtr, value); }
        }

        public bool Raycast(Vector3 origin, Vector3 unitDir, out ScriptPhysicsQueryHit hit, float maxDist)
        {
            return Internal_RayCast(mCachedPtr, ref origin, ref unitDir, out hit, maxDist);
        }

        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        private void Internal_DoOnCollisionBegin(ScriptCollisionData scriptCollisionData)
        {
            CollisionData collisionData;
            collisionData.colliderA = scriptCollisionData.colliderA.Component;
            collisionData.colliderB = scriptCollisionData.colliderB.Component;
            collisionData.contactPoints = scriptCollisionData.contactPoints;

            Component.DoOnCollisionBegin(collisionData);
        }

        private void Internal_DoOnCollisionStay(ScriptCollisionData scriptCollisionData)
        {
            CollisionData collisionData;
            collisionData.colliderA = scriptCollisionData.colliderA.Component;
            collisionData.colliderB = scriptCollisionData.colliderB.Component;
            collisionData.contactPoints = scriptCollisionData.contactPoints;

            Component.DoOnCollisionStay(collisionData);
        }

        private void Internal_DoOnCollisionEnd(ScriptCollisionData scriptCollisionData)
        {
            CollisionData collisionData;
            collisionData.colliderA = scriptCollisionData.colliderA.Component;
            collisionData.colliderB = scriptCollisionData.colliderB.Component;
            collisionData.contactPoints = scriptCollisionData.contactPoints;

            Component.DoOnCollisionEnd(collisionData);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPosition(IntPtr thisPtr, out Vector3 pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRotation(IntPtr thisPtr, out Quaternion rot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTransform(IntPtr thisPtr, ref Vector3 pos, ref Quaternion rot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetScale(IntPtr thisPtr, ref Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetScale(IntPtr thisPtr, out Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIsTrigger(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetIsTrigger(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRigidbody(IntPtr thisPtr, IntPtr rigidbody);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern NativeRigidbody Internal_GetRigidbody(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMass(IntPtr thisPtr, float mass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetMass(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaterial(IntPtr thisPtr, IntPtr physicsMaterial);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PhysicsMaterial Internal_GetMaterial(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContactOffset(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetContactOffset(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRestOffset(IntPtr thisPtr, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRestOffset(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayer(IntPtr thisPtr, ulong layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ulong Internal_GetLayer(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCollisionReportMode(IntPtr thisPtr, CollisionReportMode mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern CollisionReportMode Internal_GetCollisionReportMode(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_RayCast(IntPtr thisPtr, ref Vector3 origin, ref Vector3 unitDir, 
            out ScriptPhysicsQueryHit hit, float maxDist);
    }
}
