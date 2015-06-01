using System.IO;

namespace BansheeEngine
{
    public static class FileEx
    {
        public static void Move(string source, string destination)
        {
            string destParent = PathEx.GetParent(destination);
            if (!string.IsNullOrEmpty(destParent))
            {
                if (!Directory.Exists(destParent))
                    Directory.CreateDirectory(destParent);
            }

            File.Move(source, destination);
        }

        public static void Copy(string source, string destination)
        {
            string destParent = PathEx.GetParent(destination);
            if (!string.IsNullOrEmpty(destParent))
            {
                if (!Directory.Exists(destParent))
                    Directory.CreateDirectory(destParent);
            }

            File.Copy(source, destination);
        }
    }
}
