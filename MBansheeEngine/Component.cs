using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all components. Components represent primary logic elements in the scene. They are attached to 
    /// scene objects.
    /// </summary>
    public class Component : GameObject
    {
        // Internal use only
        protected Component()
        { }

        /// <summary>
        /// Returns the scene object this component is attached to.
        /// </summary>
        public SceneObject SceneObject
        {
            get { return Internal_GetSceneObject(mCachedPtr); }
        }

        /// <summary>
        /// Calculates bounds of the visible content for this component.
        /// </summary>
        /// <param name="box">Bounds in world space represented as an axis aligned bounding box.</param>
        /// <param name="sphere">Bounds in world space represented as a sphere.</param>
        /// <returns>True if the bounds have non-zero volume, false otherwise.</returns>
        internal protected virtual bool CalculateBounds(out AABox box, out Sphere sphere)
        {
            Vector3 pos = SceneObject.Position;

            box = new AABox(pos, pos);
            sphere = new Sphere(pos, 0.0f);

            return false;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_AddComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_GetComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component[] Internal_GetComponents(SceneObject parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_RemoveComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject Internal_GetSceneObject(IntPtr nativeInstance);
    }
}
