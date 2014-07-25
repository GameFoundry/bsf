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
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ManagedResource resource);
    }
}
