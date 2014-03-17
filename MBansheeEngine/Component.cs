using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class Component : GameObject
    {
        // Internal use only
        protected Component()
        { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_AddComponent(SceneObject parent, string ns, string type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_RemoveComponent(SceneObject parent, string ns, string type);
    }
}
