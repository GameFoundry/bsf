//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.IO;

namespace BansheeEngine
{
    /// <summary>
    /// Contains helper methods dealing with file and directory paths, extending the functionality provided by 
    /// System.IO.Path.
    /// </summary>
    public static class PathEx
    {
        /// <summary>
        /// Checks if two paths are equal.
        /// </summary>
        /// <param name="a">First path.</param>
        /// <param name="b">Second path.</param>
        /// <returns>True if both paths point to the same place.</returns>
        public static bool Compare(string a, string b)
        {
            return Path.GetFullPath(a) == Path.GetFullPath(b);
        }

        /// <summary>
        /// Checks if one path is part of an another path.
        /// </summary>
        /// <param name="path">Path to check if it's part of <paramref name="parent"/>.</param>
        /// <param name="parent">Parent that might contain <paramref name="path"/>.</param>
        /// <returns>True if <paramref name="parent"/> contains <paramref name="path"/>.</returns>
        public static bool IsPartOf(string path, string parent)
        {
            return Path.GetFullPath(path).StartsWith(Path.GetFullPath(parent));
        }

        /// <summary>
        /// Returns the last entry in the path, regardless if that is a directory or a filename.
        /// </summary>
        /// <param name="path">Path to get the tail of.</param>
        /// <returns>Tail of the path. This might be a directory or a filename.</returns>
        public static string GetTail(string path)
        {
            if (string.IsNullOrEmpty(path))
                return "";

            if (path[path.Length - 1] == Path.DirectorySeparatorChar ||
                path[path.Length - 1] == Path.AltDirectorySeparatorChar)
            {
                return Path.GetDirectoryName(path);
            }

            return Path.GetFileName(path);
        }

        /// <summary>
        /// Returns the parent of the provided path.
        /// </summary>
        /// <param name="path">Path to return the parent of.</param>
        /// <returns>Parent of the path. This usually means the folder containing the file or folder in the current path.
        /// </returns>
        public static string GetParent(string path)
        {
            string tail = GetTail(path);

            return path.Remove(path.Length - tail.Length);
        }

        /// <summary>
        /// Checks if the provided name can be used as a file name.
        /// </summary>
        /// <param name="name">Name to check.</param>
        /// <returns>True if the name can be used as a file name.</returns>
        public static bool IsValidFileName(string name)
        {
            return !string.IsNullOrWhiteSpace(name) &&
                   name.IndexOfAny(Path.GetInvalidFileNameChars()) < 0;
        }
    }
}
