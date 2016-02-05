//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all components. Components represent primary logic elements in the scene. They are attached to 
    /// scene objects.
    ///
    /// Implementations of <see cref="Component"/> can implement a set of callbacks that will be called by the runtime
    /// at specified occassions:
    /// void OnInitialize() - Called once when the component is instantiated. Only called when the game is playing.
    /// void OnUpdate() - Called every frame while the game is running and the component is enabled.
    /// void OnEnable() - Called whenever a component is enabled, or instantiated as enabled in which case it is called 
    ///                   after OnInitialize. Only called when the game is playing or paused.
    /// void OnDisable() - Called whenever a component is disabled. This includes destruction where it is called before 
    ///                    OnDestroy. Only called when the game is playing or paused.
    /// void OnDestroy() - Called before the component is destroyed. Destruction is usually delayed until the end of the 
    ///                    current frame unless specified otherwise in a call to Destroy. 
    /// void OnReset() - Called when script assemblies have been refreshed or when the component is initialized. During
    ///                  initialization it is called after OnInitialize but before OnEnable. Only relevant in editor.
    ///
    /// You can also make these callbacks trigger when the game is stopped/paused by using the <see cref="RunInEditor"/>
    /// attribute on the component.
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
        /// Destroys the component, removing it from its scene object and stopping component updates.
        /// </summary>
        /// <param name="immediate">If true the component will be fully destroyed immediately. This means that objects
        ///                         that are still referencing this component might fail. Normally destruction is delayed
        ///                         until the end of the frame to give other objects a chance to stop using it.</param>
        public void Destroy(bool immediate = false)
        {
            Internal_Destroy(mCachedPtr, immediate);
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
        internal static extern Component[] Internal_GetComponentsPerType(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_RemoveComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject Internal_GetSceneObject(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance, bool immediate);
    }
}
