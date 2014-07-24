using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class ProjectLibrary
    {
        public static void Create(Resource resource, string path)
        {
            Internal_Create(resource, path);
        }

        // TODO - Will also need (at least):
        // - GetPath
        // - Reimport
        // - Load
        // - Move
        // - Rename
        // - Delete
        // - Copy
        // - CreateFolder

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(Resource resource, string path);
    }
}
