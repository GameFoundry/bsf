//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup Rendering
     *  @{
     */

    /// <summary>
    /// Renderable represents any visible object in the scene. It has a mesh, bounds and a set of materials. Renderer will 
    /// render any Renderable objects visible by a camera.
    /// </summary>
    [RunInEditor]
    public sealed class Renderable2 : Component
    {
        /// <summary>
        /// Mesh to render. 
        /// </summary>
        public Mesh Mesh
        {
            get { return Internal_GetMesh(mCachedPtr); }
            set
            {
                IntPtr meshPtr = IntPtr.Zero;
                if (value != null)
                    meshPtr = value.GetCachedPtr();

                Internal_SetMesh(mCachedPtr, meshPtr);
            }
        }

        /// <summary>
        /// Material to use when rendering the mesh. If the mesh contains multiple sub-meshes then you may set individual
        /// materials for each sub-mesh.
        /// </summary>
        public Material Material
        {
            get { return Internal_GetMaterial(mCachedPtr, 0); }
            set
            {
                IntPtr materialPtr = IntPtr.Zero;
                if (value != null)
                    materialPtr = value.GetCachedPtr();

                Internal_SetMaterial(mCachedPtr, materialPtr, 0);
            }
        }

        /// <summary>
        /// Materials to use when rendering the mesh. 
        /// </summary>
        public Material[] Materials
        {
            get { return Internal_GetMaterials(mCachedPtr); }
            set { Internal_SetMaterials(mCachedPtr, value); }
        }

        /// <summary>
        /// Returns a material for a specific sub-mesh.
        /// </summary>
        /// <param name="index">Index of the sub-mesh.</param>
        /// <returns>Material used for rendering the sub-mesh at the specified index.</returns>
        public Material GetMaterial(int index = 0)
        {
            return Internal_GetMaterial(mCachedPtr, index);
        }

        /// <summary>
        /// Sets a material for a specific sub-mesh.
        /// </summary>
        /// <param name="material">Material to use for rendering the sub-mesh at the specified index.</param>
        /// <param name="index">Index of the sub-mesh.</param>
        public void SetMaterial(Material material, int index = 0)
        {
            IntPtr materialPtr = IntPtr.Zero;
            if (material != null)
                materialPtr = material.GetCachedPtr();

            Internal_SetMaterial(mCachedPtr, materialPtr, index);
        }

        /// <summary>
        /// Layer bitfield that controls whether a renderable is considered visible in a specific camera. Renderable layer 
        /// must match camera layer in order for the camera to render the component.
        /// </summary>
        public UInt64 Layers
        {
            get { return Internal_GetLayers(mCachedPtr); }
            set { Internal_SetLayers(mCachedPtr, value); }
        }

        /// <summary>
        /// Gets world bounds of the mesh rendered by this object.
        /// </summary>
        public Bounds Bounds
        {
            get
            {
                AABox box;
                Sphere sphere;

                Internal_GetBounds(mCachedPtr, out box, out sphere);

                return new Bounds(box, sphere);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Mesh Internal_GetMesh(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMesh(IntPtr thisPtr, IntPtr mesh);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetBounds(IntPtr thisPtr, out AABox box, out Sphere sphere);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetLayers(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayers(IntPtr thisPtr, UInt64 layers);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Material Internal_GetMaterial(IntPtr thisPtr, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaterial(IntPtr thisPtr, IntPtr material, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Material[] Internal_GetMaterials(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaterials(IntPtr thisPtr, Material[] materials);
    }

    /** @} */
}
