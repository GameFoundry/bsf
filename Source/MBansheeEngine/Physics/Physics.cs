//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Provides global access to the physics system, including scene queries and global options.
    /// </summary>
    public static class Physics
    {
        /// <summary>
        /// Global gravity value for all objects in the scene.
        /// </summary>
        public static Vector3 Gravity
        {
            get { Vector3 gravity; Internal_GetGravity(out gravity); return gravity; }
            set { Internal_SetGravity(ref value); }
        }

        /// <summary>
        /// Casts a ray into the scene and returns the closest found hit, if any.
        /// </summary>
        /// <param name="ray">Ray to cast into the scene.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool RayCast(Ray ray, out PhysicsQueryHit hit, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if (RayCast(ray.origin, ray.direction, out hit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            return false;
        }

        /// <summary>
        /// Casts a ray into the scene and returns the closest found hit, if any.
        /// </summary>
        /// <param name="origin">Origin of the ray to cast into the scene.</param>
        /// <param name="unitDir">Unit direction of the ray to cast into the scene.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool RayCast(Vector3 origin, Vector3 unitDir, out PhysicsQueryHit hit,
           ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if(Internal_RayCast(ref origin, ref unitDir, out scriptHit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            hit = new PhysicsQueryHit();
            return false;
        }

        /// <summary>
        /// Performs a sweep into the scene using a box and returns the closest found hit, if any.
        /// </summary>
        /// <param name="box">Box to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the box.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool BoxCast(AABox box, Quaternion rotation, Vector3 unitDir, out PhysicsQueryHit hit,
            ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if(Internal_BoxCast(ref box, ref rotation, ref unitDir, out scriptHit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            hit = new PhysicsQueryHit();
            return false;
        }

        /// <summary>
        /// Performs a sweep into the scene using a sphere and returns the closest found hit, if any.
        /// </summary>
        /// <param name="sphere">Sphere to sweep through the scene.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool SphereCast(Sphere sphere, Vector3 unitDir, out PhysicsQueryHit hit,
            ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if(Internal_SphereCast(ref sphere, ref unitDir, out scriptHit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            hit = new PhysicsQueryHit();
            return false;
        }

        /// <summary>
        /// Performs a sweep into the scene using a capsule and returns the closest found hit, if any.
        /// </summary>
        /// <param name="capsule">Capsule to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the capsule.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool CapsuleCast(Capsule capsule, Quaternion rotation, Vector3 unitDir,
            out PhysicsQueryHit hit, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if(Internal_CapsuleCast(ref capsule, ref rotation, ref unitDir, out scriptHit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            hit = new PhysicsQueryHit();
            return false;
        }

        /// <summary>
        /// Performs a sweep into the scene using a convex mesh and returns the closest found hit, if any.
        /// </summary>
        /// <param name="mesh">Mesh to sweep through the scene. Must be convex.</param>
        /// <param name="position">Starting position of the mesh.</param>
        /// <param name="rotation">Orientation of the mesh.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="hit">Information recorded about a hit. Only valid if method returns true.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool ConvexCast(PhysicsMesh mesh, Vector3 position, Quaternion rotation,
            Vector3 unitDir, out PhysicsQueryHit hit, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            IntPtr meshPtr = IntPtr.Zero;
            if (mesh != null)
                meshPtr = mesh.GetCachedPtr();

            ScriptPhysicsQueryHit scriptHit = new ScriptPhysicsQueryHit();
            if(Internal_ConvexCast(meshPtr, ref position, ref rotation, ref unitDir, out scriptHit, layer, max))
            {
                ConvertPhysicsQueryHit(ref scriptHit, out hit);
                return true;
            }

            hit = new PhysicsQueryHit();
            return false;
        }

        /// <summary>
        /// Casts a ray into the scene and returns all found hits.
        /// </summary>
        /// <param name="ray">Ray to cast into the scene.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] RayCastAll(Ray ray, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return RayCastAll(ray.origin, ray.direction, layer, max);
        }

        /// <summary>
        /// Casts a ray into the scene and returns all found hits.
        /// </summary>
        /// <param name="origin">Origin of the ray to cast into the scene.</param>
        /// <param name="unitDir">Unit direction of the ray to cast into the scene.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] RayCastAll(Vector3 origin, Vector3 unitDir,
			ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return ConvertPhysicsQueryHits(Internal_RayCastAll(ref origin, ref unitDir, layer, max));
        }

        /// <summary>
        /// Performs a sweep into the scene using a box and returns all found hits.
        /// </summary>
        /// <param name="box">Box to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the box.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] BoxCastAll(AABox box, Quaternion rotation,
            Vector3 unitDir, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return ConvertPhysicsQueryHits(Internal_BoxCastAll(ref box, ref rotation, ref unitDir, layer, max));
        }

        /// <summary>
        /// Performs a sweep into the scene using a sphere and returns all found hits.
        /// </summary>
        /// <param name="sphere">Sphere to sweep through the scene.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] SphereCastAll(Sphere sphere, Vector3 unitDir,
            ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return ConvertPhysicsQueryHits(Internal_SphereCastAll(ref sphere, ref unitDir, layer, max));
        }

        /// <summary>
        /// Performs a sweep into the scene using a capsule and returns all found hits.
        /// </summary>
        /// <param name="capsule">Capsule to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the capsule.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] CapsuleCastAll(Capsule capsule, Quaternion rotation,
            Vector3 unitDir, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return ConvertPhysicsQueryHits(Internal_CapsuleCastAll(ref capsule, ref rotation, ref unitDir, layer, max));
        }

        /// <summary>
        /// Performs a sweep into the scene using a convex mesh and returns all found hits.
        /// </summary>
        /// <param name="mesh">Mesh to sweep through the scene. Must be convex.</param>
        /// <param name="position">Starting position of the mesh.</param>
        /// <param name="rotation">Orientation of the mesh.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>List of all detected hits.</returns>
        public static PhysicsQueryHit[] ConvexCastAll(PhysicsMesh mesh, Vector3 position,
            Quaternion rotation, Vector3 unitDir, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            IntPtr meshPtr = IntPtr.Zero;
            if (mesh != null)
                meshPtr = mesh.GetCachedPtr();

            return ConvertPhysicsQueryHits(Internal_ConvexCastAll(meshPtr, ref position, ref rotation, ref unitDir, layer, max));
        }

        /// <summary>
        /// Casts a ray into the scene and checks if it has hit anything. This can be significantly more efficient than 
        /// other types of cast* calls.
        /// </summary>
        /// <param name="ray">Ray to cast into the scene.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool RayCastAny(Ray ray, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return RayCastAny(ray.origin, ray.direction, layer, max);
        }

        /// <summary>
        /// Casts a ray into the scene and checks if it has hit anything. This can be significantly more efficient than 
        /// other types of cast* calls.
        /// </summary>
        /// <param name="origin">Origin of the ray to cast into the scene.</param>
        /// <param name="unitDir">Unit direction of the ray to cast into the scene.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool RayCastAny(Vector3 origin, Vector3 unitDir, ulong layer = ulong.MaxValue, 
            float max = float.MaxValue)
        {
            return Internal_RayCastAny(ref origin, ref unitDir, layer, max);
        }

        /// <summary>
        /// Performs a sweep into the scene using a box and checks if it has hit anything. This can be significantly more 
        /// efficient than other types of cast* calls.
        /// </summary>
        /// <param name="box">Box to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the box.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool BoxCastAny(AABox box, Quaternion rotation, Vector3 unitDir, ulong layer = ulong.MaxValue, 
            float max = float.MaxValue)
        {
            return Internal_BoxCastAny(ref box, ref rotation, ref unitDir, layer, max);
        }

        /// <summary>
        /// Performs a sweep into the scene using a sphere and checks if it has hit anything. This can be significantly more
        /// efficient than other types of cast* calls.
        /// </summary>
        /// <param name="sphere">Sphere to sweep through the scene.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool SphereCastAny(Sphere sphere, Vector3 unitDir, ulong layer = ulong.MaxValue, 
            float max = float.MaxValue)
        {
            return Internal_SphereCastAny(ref sphere, ref unitDir, layer, max);
        }

        /// <summary>
        /// Performs a sweep into the scene using a capsule and checks if it has hit anything. This can be significantly 
        /// more efficient than other types of cast* calls.
        /// </summary>
        /// <param name="capsule">Capsule to sweep through the scene.</param>
        /// <param name="rotation">Orientation of the capsule.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool CapsuleCastAny(Capsule capsule, Quaternion rotation, Vector3 unitDir,
            ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            return Internal_CapsuleCastAny(ref capsule, ref rotation, ref unitDir, layer, max);
        }

        /// <summary>
        /// Performs a sweep into the scene using a convex mesh and checks if it has hit anything. This can be significantly
        /// more efficient than other types of cast* calls.
        /// </summary>
        /// <param name="mesh">Mesh to sweep through the scene. Must be convex.</param>
        /// <param name="position">Starting position of the mesh.</param>
        /// <param name="rotation">Orientation of the mesh.</param>
        /// <param name="unitDir">Unit direction towards which to perform the sweep.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <param name="max">Maximum distance at which to perform the query. Hits past this distance will not be detected.
        ///                   </param>
        /// <returns>True if something was hit, false otherwise.</returns>
        public static bool ConvexCastAny(PhysicsMesh mesh, Vector3 position, Quaternion rotation,
            Vector3 unitDir, ulong layer = ulong.MaxValue, float max = float.MaxValue)
        {
            IntPtr meshPtr = IntPtr.Zero;
            if (mesh != null)
                meshPtr = mesh.GetCachedPtr();

            return Internal_ConvexCastAny(meshPtr, ref position, ref rotation, ref unitDir, layer, max);
        }

        /// <summary>
        /// Returns a list of all colliders in the scene that overlap the provided box.
        /// </summary>
        /// <param name="box">Box to check for overlap.</param>
        /// <param name="rotation">Orientation of the box.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>List of all colliders that overlap the box.</returns>
        public static Collider[] BoxOverlap(AABox box, Quaternion rotation, ulong layer = ulong.MaxValue)
        {
            return ConvertColliders(Internal_BoxOverlap(ref box, ref rotation, layer));
        }

        /// <summary>
        /// Returns a list of all colliders in the scene that overlap the provided sphere.
        /// </summary>
        /// <param name="sphere">Sphere to check for overlap.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>List of all colliders that overlap the sphere.</returns>
        public static Collider[] SphereOverlap(Sphere sphere, ulong layer = ulong.MaxValue)
        {
            return ConvertColliders(Internal_SphereOverlap(ref sphere, layer));
        }

        /// <summary>
        /// Returns a list of all colliders in the scene that overlap the provided capsule.
        /// </summary>
        /// <param name="capsule">Capsule to check for overlap.</param>
        /// <param name="rotation">Orientation of the capsule.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>List of all colliders that overlap the sphere.</returns>
        public static Collider[] CapsuleOverlap(Capsule capsule, Quaternion rotation, ulong layer = ulong.MaxValue)
        {
            return ConvertColliders(Internal_CapsuleOverlap(ref capsule, ref rotation, layer));
        }

        /// <summary>
        /// Returns a list of all colliders in the scene that overlap the provided convex mesh.
        /// </summary>
        /// <param name="mesh">Mesh to check for overlap. Must be convex.</param>
        /// <param name="position">Position of the mesh.</param>
        /// <param name="rotation">Orientation of the mesh.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>List of all colliders that overlap the mesh.</returns>
        public static Collider[] ConvexOverlap(PhysicsMesh mesh, Vector3 position, Quaternion rotation, 
            ulong layer = ulong.MaxValue)
        {
            IntPtr meshPtr = IntPtr.Zero;
            if (mesh != null)
                meshPtr = mesh.GetCachedPtr();

            return ConvertColliders(Internal_ConvexOverlap(meshPtr, ref position, ref rotation, layer));
        }

        /// <summary>
        /// Checks if the provided box overlaps any other collider in the scene.
        /// </summary>
        /// <param name="box">Box to check for overlap.</param>
        /// <param name="rotation">Orientation of the box.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>True if there is overlap with another object, false otherwise.</returns>
        public static bool BoxOverlapAny(AABox box, Quaternion rotation, ulong layer = ulong.MaxValue)
        {
            return Internal_BoxOverlapAny(ref box, ref rotation, layer);
        }

        /// <summary>
        /// Checks if the provided sphere overlaps any other collider in the scene.
        /// </summary>
        /// <param name="sphere">Sphere to check for overlap.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>True if there is overlap with another object, false otherwise.</returns>
        public static bool SphereOverlapAny(Sphere sphere, ulong layer = ulong.MaxValue)
        {
            return Internal_SphereOverlapAny(ref sphere, layer);
        }

        /// <summary>
        /// Checks if the provided capsule overlaps any other collider in the scene.
        /// </summary>
        /// <param name="capsule">Capsule to check for overlap.</param>
        /// <param name="rotation">Orientation of the capsule.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>True if there is overlap with another object, false otherwise.</returns>
        public static bool CapsuleOverlapAny(Capsule capsule, Quaternion rotation, ulong layer = ulong.MaxValue)
        {
            return Internal_CapsuleOverlapAny(ref capsule, ref rotation, layer);
        }

        /// <summary>
        /// Checks if the provided convex mesh overlaps any other collider in the scene.
        /// </summary>
        /// <param name="mesh">Mesh to check for overlap. Must be convex.</param>
        /// <param name="position">Position of the mesh.</param>
        /// <param name="rotation">Orientation of the mesh.</param>
        /// <param name="layer">Layers to consider for the query. This allows you to ignore certain groups of objects.
        ///                     </param>
        /// <returns>True if there is overlap with another object, false otherwise.</returns>
        public static bool ConvexOverlapAny(PhysicsMesh mesh, Vector3 position, Quaternion rotation,
            ulong layer = ulong.MaxValue)
        {
            IntPtr meshPtr = IntPtr.Zero;
            if (mesh != null)
                meshPtr = mesh.GetCachedPtr();

            return Internal_ConvexOverlapAny(meshPtr, ref position, ref rotation, layer);
        }

        /// <summary>
        /// Adds a new physics region. Certain physics options require you to set up regions in which physics objects are
        /// allowed to be in, and objects outside of these regions will not be handled by physics.You do not need to set
        /// up these regions by default.
        /// </summary>
        /// <param name="region">Region of space that physics objects are allowed to be in.</param>
        /// <returns>A unique handle to the region.</returns>
        public static int AddPhysicsRegion(AABox region)
        {
            return Internal_AddPhysicsRegion(ref region);
        }

        /// <summary>
        /// Removes a physics region.
        /// </summary>
        /// <param name="handle">Handle of the region returned by <see cref="AddPhysicsRegion"/></param>
        public static void RemovePhysicsRegion(int handle)
        {
            Internal_RemovePhysicsRegion(handle);
        }

        /// <summary>
        /// Removes all physics regions. 
        /// </summary>
        public static void ClearPhysicsRegions()
        {
            Internal_ClearPhysicsRegions();
        }

        /// <summary>
        /// Enables or disables collision between two layers. Each physics object can be assigned a specific layer, and here
        /// you can determine which layers can interact with each other.
        /// </summary>
        /// <param name="layerA">First collision layer.</param>
        /// <param name="layerB">Second collision layer.</param>
        /// <param name="enabled">True if the layers are allowed to interact, false otherwise.</param>
        public static void ToggleCollision(ulong layerA, ulong layerB, bool enabled)
        {
            Internal_ToggleCollision(layerA, layerB, enabled);
        }

        /// <summary>
        /// Checks if two collision layers are allowed to interact.
        /// </summary>
        /// <param name="layerA">First collision layer.</param>
        /// <param name="layerB">Second collision layer.</param>
        /// <returns>True if the two provided layers are allowed to interact.</returns>
        public static bool IsCollisionEnabled(ulong layerA, ulong layerB)
        {
            return Internal_IsCollisionEnabled(layerA, layerB);
        }

        /// <summary>
        /// Checks is the physics simulation update currently in progress.
        /// </summary>
        internal static bool IsUpdateInProgress
        {
            get { return Internal_IsUpdateInProgress(); }
        }

        /// <summary>
        /// Converts a physics query hit info retrieved from native code into managed physics query hit.
        /// </summary>
        /// <param name="scriptHit">Native physics query hit info.</param>
        /// <param name="hit">Managed physics query hit info</param>
        private static void ConvertPhysicsQueryHit(ref ScriptPhysicsQueryHit scriptHit, out PhysicsQueryHit hit)
        {
            hit.collider = scriptHit.collider.Component;
            hit.distance = scriptHit.distance;
            hit.normal = scriptHit.normal;
            hit.point = scriptHit.point;
            hit.triangleIdx = scriptHit.triangleIdx;
            hit.uv = scriptHit.uv;
        }

        /// <summary>
        /// Converts all provided physics query hit infos retrieved from native code into managed physics query hits.
        /// </summary>
        /// <param name="scriptHits">Native physics query hits.</param>
        /// <returns>Converted managed physics query hits.</returns>
        private static PhysicsQueryHit[] ConvertPhysicsQueryHits(ScriptPhysicsQueryHit[] scriptHits)
        {
            PhysicsQueryHit[] output = new PhysicsQueryHit[scriptHits.Length];

            for (int i = 0; i < scriptHits.Length; i++)
                ConvertPhysicsQueryHit(ref scriptHits[i], out output[i]);

            return output;
        }

        /// <summary>
        /// Converts an array of native colliders to collider components.
        /// </summary>
        /// <param name="colliders">Native colliders.</param>
        /// <returns>Managed collider components.</returns>
        private static Collider[] ConvertColliders(NativeCollider[] colliders)
        {
            Collider[] output = new Collider[colliders.Length];
            for (int i = 0; i < colliders.Length; i++)
                output[i] = colliders[i].Component;

            return output;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetGravity(out Vector3 gravity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetGravity(ref Vector3 gravity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_AddPhysicsRegion(ref AABox region);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RemovePhysicsRegion(int handle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearPhysicsRegions();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ToggleCollision(ulong layerA, ulong layerB, bool enabled);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsCollisionEnabled(ulong layerA, ulong layerB);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsUpdateInProgress();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_RayCast(ref Vector3 origin, ref Vector3 unitDir, out ScriptPhysicsQueryHit hit, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_BoxCast(ref AABox box, ref Quaternion rotation, ref Vector3 unitDir, out ScriptPhysicsQueryHit hit, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_SphereCast(ref Sphere sphere, ref Vector3 unitDir, out ScriptPhysicsQueryHit hit, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_CapsuleCast(ref Capsule capsule, ref Quaternion rotation, ref Vector3 unitDir, out ScriptPhysicsQueryHit hit, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_ConvexCast(IntPtr mesh, ref Vector3 position, ref Quaternion rotation, ref Vector3 unitDir, out ScriptPhysicsQueryHit hit, ulong layer, float max);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ScriptPhysicsQueryHit[] Internal_RayCastAll(ref Vector3 origin, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ScriptPhysicsQueryHit[] Internal_BoxCastAll(ref AABox box, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ScriptPhysicsQueryHit[] Internal_SphereCastAll(ref Sphere sphere, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ScriptPhysicsQueryHit[] Internal_CapsuleCastAll(ref Capsule capsule, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ScriptPhysicsQueryHit[] Internal_ConvexCastAll(IntPtr mesh, ref Vector3 position, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_RayCastAny(ref Vector3 origin, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_BoxCastAny(ref AABox box, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_SphereCastAny(ref Sphere sphere, ref Vector3 unitDir, ulong layer,  float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_CapsuleCastAny(ref Capsule capsule, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_ConvexCastAny(IntPtr mesh, ref Vector3 position, ref Quaternion rotation, ref Vector3 unitDir, ulong layer, float max);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern NativeCollider[] Internal_BoxOverlap(ref AABox box, ref Quaternion rotation, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern NativeCollider[] Internal_SphereOverlap(ref Sphere sphere, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern NativeCollider[] Internal_CapsuleOverlap(ref Capsule capsule, ref Quaternion rotation, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern NativeCollider[] Internal_ConvexOverlap(IntPtr mesh, ref Vector3 position, ref Quaternion rotation, ulong layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_BoxOverlapAny(ref AABox box, ref Quaternion rotation, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_SphereOverlapAny(ref Sphere sphere, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_CapsuleOverlapAny(ref Capsule capsule, ref Quaternion rotation, ulong layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_ConvexOverlapAny(IntPtr mesh, ref Vector3 position, ref Quaternion rotation, ulong layer);
    }
}
