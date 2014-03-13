using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class Component : ScriptObject
    {
        public Component()
        {
            Internal_CreateInstance(this, GetType().Namespace, GetType().Name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Component instance, string ns, string type);

        private int dbgBase;
    }
}
