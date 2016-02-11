//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Mesh that is used purely for collision purposes and not rendering. For example as a collider or a trigger.
    /// </summary>
    public class PhysicsMesh : Resource
    {
        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        private PhysicsMesh()
        { }

        /// <summary>
        /// Retrieves the vertex and index data of the mesh.
        /// </summary>
        public MeshData MeshData
        {
            get { return Internal_GetMeshData(mCachedPtr); }
        }

        /// <summary>
        /// Returns the type of the mesh.
        /// </summary>
        public PhysicsMeshType MeshType
        {
            get { return (PhysicsMeshType)Internal_GetMeshType(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern MeshData Internal_GetMeshData(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetMeshType(IntPtr thisPtr);
    }

    /// <summary>
    /// Valid types of a mesh used for physics.
    /// </summary>
    public enum PhysicsMeshType
    {
        /// <summary>
        /// A regular triangle mesh. Mesh can be of arbitrary size but cannot be used for triggers and non-kinematic 
        /// objects. Occurs a significantly larger performance impact than convex meshes.
        /// </summary>
        Triangle,
        /// <summary>
        /// Mesh representing a convex shape. Mesh will not have more than 256 vertices. Occurs a significantly lower
        /// performance impact than triangle meshes.
        /// </summary>
        Convex

    }
}