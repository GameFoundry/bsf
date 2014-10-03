using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class Component : GameObject
    {
        // Internal use only
        protected Component()
        { }

        public SceneObject sceneObject
        {
            get { throw new NotImplementedException(); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_AddComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_GetComponent(SceneObject parent, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component[] Internal_GetComponents(SceneObject parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_RemoveComponent(SceneObject parent, Type type);
    }
}
