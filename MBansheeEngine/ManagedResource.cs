using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class ManagedResource : Resource
    {
        // For internal use by the runtime
        protected ManagedResource()
        { }

        static public T Create<T>() where T : ManagedResource, new()
        {
            T newResource = new T();
            Internal_CreateInstance(newResource);

            return newResource;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ManagedResource resource);
    }
}
