using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native Renderable class.
    /// <see cref="Renderable"/>
    /// </summary>
    internal class NativeRenderable : ScriptObject
    {
        internal Mesh Mesh
        {
            get
            {
                return mesh; 
            }
            set
            {
                mesh = value;

                int subMeshCount = 0;
                if (mesh != null)
                    subMeshCount = mesh.SubMeshCount;

                Material[] newMaterials = new Material[subMeshCount];
                int numToCopy = MathEx.Min(newMaterials.Length, materials.Length);
                Array.Copy(materials, newMaterials, numToCopy);
                materials = newMaterials;

                IntPtr meshPtr = IntPtr.Zero;
                if (mesh != null)
                    meshPtr = mesh.GetCachedPtr();

                Internal_SetMesh(mCachedPtr, meshPtr); 
                
            }
        }

        internal Bounds GetBounds(SceneObject parent)
        {
            AABox box;
            Sphere sphere;

            Internal_GetBounds(mCachedPtr, parent.mCachedPtr, out box, out sphere);

            return new Bounds(box, sphere);
        }

        internal UInt64 Layers
        {
            get { return Internal_GetLayers(mCachedPtr); }
            set { Internal_SetLayers(mCachedPtr, value); }
        }

        private Material[] materials = new Material[0];
        private Mesh mesh;
        
        public NativeRenderable(SceneObject sceneObject)
        {
            IntPtr sceneObjPtr = IntPtr.Zero;
            if (sceneObject != null)
                sceneObjPtr = sceneObject.GetCachedPtr();

            Internal_Create(this, sceneObjPtr);
        }

        internal Material[] Materials
        {
            get { return materials; }
            set
            {
                int newNumMaterials = value != null ? value.Length : 0;
                int min = MathEx.Min(materials.Length, newNumMaterials);

                for (int i = 0; i < min; i++)
                    materials[i] = value[i];

                for (int i = min; i < materials.Length; i++)
                    materials[i] = null;

                Internal_SetMaterials(mCachedPtr, materials);
            }
        }

        internal Material GetMaterial(int index = 0)
        {
            return materials[index];
        }

        internal void SetMaterial(Material material, int index = 0)
        {
            materials[index] = material;

            IntPtr materialPtr = IntPtr.Zero;
            if (material != null)
                materialPtr = material.GetCachedPtr();

            Internal_SetMaterial(mCachedPtr, materialPtr, index);
        }

        internal void UpdateTransform(SceneObject sceneObject)
        {
            Internal_UpdateTransform(mCachedPtr, sceneObject.mCachedPtr);
        }
        
        internal void OnDestroy()
        {
            Internal_OnDestroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(NativeRenderable instance, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateTransform(IntPtr thisPtr, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMesh(IntPtr thisPtr, IntPtr mesh);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetBounds(IntPtr thisPtr, IntPtr parentSO, out AABox box, out Sphere sphere);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetLayers(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayers(IntPtr thisPtr, UInt64 layers);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaterial(IntPtr thisPtr, IntPtr material, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaterials(IntPtr thisPtr, Material[] materials);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_OnDestroy(IntPtr thisPtr);
    }
}
