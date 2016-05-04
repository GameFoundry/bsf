//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Physics
     *  @{
     */

    /// <summary>
    /// Hit information from a physics query.
    /// </summary>
    public struct PhysicsQueryHit
    {
        /// <summary>
        /// Position of the hit in world space.
        /// </summary>
        public Vector3 point;

        /// <summary>
        /// Normal to the surface that was hit. 
        /// </summary>
        public Vector3 normal;

        /// <summary>
        /// UV coordinates of the triangle that was hit (only applicable when triangle meshes are hit).
        /// </summary>
        public Vector2 uv;

        /// <summary>
        /// Distance from the query origin to the hit position.
        /// </summary>
        public float distance;

        /// <summary>
        /// Index of the triangle that was hit (only applicable when triangle meshes are hit).
        /// </summary>
        public int triangleIdx;

        /// <summary>
        /// Collider that was hit.
        /// </summary>
        public Collider collider;
    }

    /// <summary>
    /// Information about a single contact point during physics collision.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ContactPoint // Note: Must match C++ ContactPoint struct
    {
        /// <summary>
        /// Contact point in world space.
        /// </summary>
        public Vector3 position;

        /// <summary>
        /// Normal pointing from the second shape to the first shape.
        /// </summary>
        public Vector3 normal;

        /// <summary>
        /// Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force.
        /// </summary>
        public float impulse;

        /// <summary>
        /// Determines how far are the objects. Negative value denotes penetration.
        /// </summary>
        public float separation;
    }

    /// <summary>
    /// Information about a collision between two physics objects.
    /// </summary>
    public struct CollisionData
    {
        internal CollisionData(ScriptCollisionData data)
        {
            if (data.colliderA != null)
                colliderA = data.colliderA.Component;
            else
                colliderA = null;

            if (data.colliderB != null)
                colliderB = data.colliderB.Component;
            else
                colliderB = null;

            contactPoints = data.contactPoints;
        }

        /// <summary>
        /// First of the colliders involved in the collision.
        /// </summary>
        public Collider colliderA;

        /// <summary>
        /// Second of the colliders involved in the collision.
        /// </summary>
        public Collider colliderB;

        /// <summary>
        /// Information about all the contact points for the hit. 
        /// </summary>
        public ContactPoint[] contactPoints;
    }

    /// <summary>
    /// Interop class used for passing PhysicsQueryHit data from native to managed code. <see cref="PhysicsQueryHit"/>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptPhysicsQueryHit // Note: Must match C++ struct ScriptPhysicsQueryHit
    {
        public Vector3 point;
        public Vector3 normal;
        public Vector2 uv;
        public float distance;
        public int triangleIdx;
        public NativeCollider collider;
    }

    /// <summary>
    /// Interop class used for passing CollisionData data from native to managed code. <see cref="CollisionData"/>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptCollisionData // Note: Must match C++ CollisionData struct
    {
        public NativeCollider colliderA;
        public NativeCollider colliderB;
        public ContactPoint[] contactPoints;
    }

    /// <summary>
    /// Determines which collision events will be reported by physics objects.
    /// </summary>
    public enum CollisionReportMode
    {
        /// <summary>
        /// No collision events will be triggered.
        /// </summary>
        None,
        /// <summary>
        /// Collision events will be triggered when object enters and/or leaves collision.
        /// </summary>
		Report,
		/// <summary>
        /// Collision events will be triggered when object enters and/or leaves collision, but also every frame the object 
        /// remains in collision. 
        /// </summary>
		ReportPersistent, 
	}

    /** @} */
}
