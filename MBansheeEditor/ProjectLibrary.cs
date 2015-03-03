using System;
using System.IO;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class ProjectLibrary : ScriptObject
    {
        public static DirectoryEntry Root { get { return Internal_GetRoot(); } }

        public static void Create(Resource resource, string path)
        {
            if (Path.IsPathRooted(path))
                throw new ArgumentException("Provided path must be relative.", "path");

            Internal_Create(resource, path);
        }

        public static void Save(Resource resource)
        {
            Internal_Save(resource);
        }

        public static T Load<T>(string path) where T : Resource
        {
            if (Path.IsPathRooted(path))
                throw new ArgumentException("Provided path must be relative.", "path");

            return (T) Internal_Load(path);
        }

        public static void Reimport(string path, ImportOptions options = null, bool force = false)
        {
            Internal_Reimport(path, options, force);
        }

        public static bool Exists(string path)
        {
            return GetEntry(path) != null;
        }

        public static LibraryEntry GetEntry(string path)
        {
            return Internal_GetEntry(path);
        }

        public static string GetPath(Resource resource)
        {
            return Internal_GetPath(resource);
        }

        public static void Delete(string path)
        {
            Internal_Delete(path);
        }

        public static void CreateFolder(string path)
        {
            Internal_CreateFolder(path);
        }

        public static void Rename(string path, string name)
        {
            Internal_Rename(path, name);
        }

        public static void Move(string oldPath, string newPath, bool overwrite = false)
        {
            Internal_Move(oldPath, newPath, overwrite);
        }

        public static void Copy(string source, string destination, bool overwrite = false)
        {
            Internal_Copy(source, destination, overwrite);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(Resource resource, string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_Load(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Save(Resource resource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern DirectoryEntry Internal_GetRoot();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Reimport(string path, ImportOptions options, bool force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern LibraryEntry Internal_GetEntry(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetPath(Resource resource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Delete(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateFolder(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Rename(string path, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Move(string oldPath, string newPath, bool overwrite);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Copy(string source, string destination, bool overwrite);
    }

    public enum LibraryEntryType
    {
        File, Directory
    }

    public enum ResourceType
    {
        Texture, SpriteTexture, Mesh, Font, GUISkin
    }

    public class LibraryEntry : ScriptObject
    {
        public String Path
        {
            get
            {
                // TODO
                return "";
            }
        }

        public String Name
        {
            get
            {
                // TODO
                return "";
            }
        }

        public LibraryEntryType Type
        {
            get
            {
                // TODO
                return LibraryEntryType.File;
            }
        }

        public DirectoryEntry Parent
        {
            get
            {
                // TODO
                return null;
            }
        }
    }

    public class DirectoryEntry : LibraryEntry
    {
        public LibraryEntry[] Children
        {
            get
            {
                // TODO
                return null;
            }
        }
    }

    public class FileEntry : LibraryEntry
    {
        public ImportOptions Options
        {
            get
            {
                // TODO
                return null;
            }
        }

        public string UUID
        {
            get
            {
                // TODO
                return "";
            }
        }

        public Texture2D Icon
        {
            get
            {
                // TODO
                return null;
            }
        }

        public ResourceType ResType
        {
            get
            {
                // TODO
                return ResourceType.Texture;
            }
        }
    }
}
