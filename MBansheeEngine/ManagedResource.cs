using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class ManagedResource : Resource
    {
        public ManagedResource()
        {
            Internal_CreateResource(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateResource(ManagedResource resource);
    }
}
