using System.IO;

namespace BansheeEngine
{
    public static class PathEx
    {
        public static bool Compare(string a, string b)
        {
            return Path.GetFullPath(a) == Path.GetFullPath(b);
        }

        public static bool IsPartOf(string path, string parent)
        {
            return Path.GetFullPath(path).StartsWith(Path.GetFullPath(parent));
        }

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

        public static string GetParent(string path)
        {
            string tail = GetTail(path);

            return path.Remove(path.Length - tail.Length);
        }
    }
}
