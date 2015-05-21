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
    }
}
