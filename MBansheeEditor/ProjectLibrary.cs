using System;
using System.IO;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class ProjectLibrary
    {
        public static void Create(Resource resource, string path)
        {
            if (Path.IsPathRooted(path))
                throw new ArgumentException("Provided path must be relative.", "path");

            Internal_Create(resource, path);
        }

        public static T Load<T>(string path) where T : Resource
        {
            if (Path.IsPathRooted(path))
                throw new ArgumentException("Provided path must be relative.", "path");

            return (T) Internal_Load(path);
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_Load(string path);
    }
}
