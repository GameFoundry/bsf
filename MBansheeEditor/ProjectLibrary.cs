using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class ProjectLibrary : ScriptObject
    {
        public static DirectoryEntry Root { get { return Internal_GetRoot(); } }
        public static string ResourceFolder { get { return Internal_GetResourceFolder(); } }

        public static event Action<string> OnEntryAdded;
        public static event Action<string> OnEntryRemoved;

        private static HashSet<string> queuedForImport = new HashSet<string>();
        private static int numImportedFiles;
        private static int totalFilesToImport;

        private const float TIME_SLICE_SECONDS = 0.030f;

        public static void Refresh()
        {
            string[] modifiedPaths = Internal_Refresh(ResourceFolder, false);
            foreach (var modifiedPath in modifiedPaths)
            {
                if (queuedForImport.Add(modifiedPath))
                    totalFilesToImport++;
            }
        }

        public static void Refresh(string path)
        {
            string[] modifiedPaths = Internal_Refresh(path, false);
            foreach (var modifiedPath in modifiedPaths)
            {
                if (queuedForImport.Add(modifiedPath))
                    totalFilesToImport++;
            }
        }

        public static void Create(Resource resource, string path)
        {
            Internal_Create(resource, path);
        }

        public static void Save(Resource resource)
        {
            Internal_Save(resource);
        }

        public static T Load<T>(string path) where T : Resource
        {
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

        public static LibraryEntry[] Search(string pattern, ResourceType[] types = null)
        {
            return Internal_Search(pattern, types);
        }

        public static string GetPath(Resource resource)
        {
            return Internal_GetPath(resource);
        }

        public static string GetPath(string uuid)
        {
            return Internal_GetPathFromUUID(uuid);
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

        internal static void Update()
        {
            if (queuedForImport.Count > 0)
            {
                UInt64 start = Time.Precise;
                List<string> toRemove = new List<string>();
                string lastEntry = "";

                foreach (var entry in queuedForImport)
                {
                    lastEntry = entry;

                    Internal_Refresh(entry, true);
                    toRemove.Add(entry);
                    numImportedFiles++;

                    UInt64 end = Time.Precise;
                    UInt64 elapsed = end - start;

                    float elapsedSeconds = elapsed * Time.MicroToSecond;
                    if (elapsedSeconds > TIME_SLICE_SECONDS)
                        break;
                }

                foreach (var entry in toRemove)
                    queuedForImport.Remove(entry);

                if (queuedForImport.Count == 0)
                {
                    numImportedFiles = 0;
                    totalFilesToImport = 0;

                    ProgressBar.Hide();
                }
                else
                {
                    string displayName = lastEntry;
                    displayName = displayName.Replace("\\", "\\\\");

                    if (displayName.Length > 60)
                    {
                        displayName = displayName.Remove(0, displayName.Length - 60);
                        displayName = "..." + displayName;
                    }

                    float pct = numImportedFiles / (float)totalFilesToImport;
                    ProgressBar.Show("Importing (" + numImportedFiles + "/" + totalFilesToImport + ")", displayName, pct);
                }
            }
        }

        private static void Internal_DoOnEntryAdded(string path)
        {
            if (OnEntryAdded != null)
                OnEntryAdded(path);
        }

        private static void Internal_DoOnEntryRemoved(string path)
        {
            if (OnEntryRemoved != null)
                OnEntryRemoved(path);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_Refresh(string path, bool import);

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
        private static extern LibraryEntry[] Internal_Search(string path, ResourceType[] types);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetPath(Resource resource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetPathFromUUID(string uuid);

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetResourceFolder();
    }

    // Note: Must be the same as C++ enum ProjectLibrary::LibraryEntryType
    public enum LibraryEntryType
    {
        File, Directory
    }

    // Note: Must be the same as C++ enum ScriptResourceType
    public enum ResourceType
    {
        Texture, SpriteTexture, Mesh, Font, Shader, Material, Prefab, PlainText, ScriptCode, StringTable, Undefined
    }

    public class LibraryEntry : ScriptObject
    {
        public string Path { get { return Internal_GetPath(mCachedPtr); } }
        public string Name { get { return Internal_GetName(mCachedPtr); } }

        public LibraryEntryType Type { get { return Internal_GetType(mCachedPtr); } }
        public DirectoryEntry Parent { get { return Internal_GetParent(mCachedPtr); } }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetPath(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetName(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern LibraryEntryType Internal_GetType(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern DirectoryEntry Internal_GetParent(IntPtr thisPtr);
    }

    public class DirectoryEntry : LibraryEntry
    {
        public LibraryEntry[] Children { get { return Internal_GetChildren(mCachedPtr); } }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern LibraryEntry[] Internal_GetChildren(IntPtr thisPtr);
    }

    public class FileEntry : LibraryEntry
    {
        public ImportOptions Options { get { return Internal_GetImportOptions(mCachedPtr); } }
        public string UUID { get { return Internal_GetUUID(mCachedPtr); } }
        public Texture2D Icon { get { return Internal_GetIcon(mCachedPtr); } }
        public ResourceType ResType { get { return Internal_GetResourceType(mCachedPtr); } }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ImportOptions Internal_GetImportOptions(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetUUID(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetIcon(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ResourceType Internal_GetResourceType(IntPtr thisPtr);
    }
}
