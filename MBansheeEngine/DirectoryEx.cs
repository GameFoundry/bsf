using System.IO;

namespace BansheeEngine
{
    public static class DirectoryEx
    {
        public static void Move(string source, string destination)
        {
            string destParent = PathEx.GetParent(destination);
            if (!string.IsNullOrEmpty(destParent))
            {
                if (!Directory.Exists(destParent))
                    Directory.CreateDirectory(destParent);
            }

            Directory.Move(source, destination);
        }

        public static void Copy(string source, string destination)
        {
            DirectoryInfo dir = new DirectoryInfo(source);
            DirectoryInfo[] dirs = dir.GetDirectories();

            if (!dir.Exists)
            {
                throw new DirectoryNotFoundException(
                    "Source directory does not exist or could not be found: "
                    + source);
            }

            if (!Directory.Exists(destination))
                Directory.CreateDirectory(destination);

            FileInfo[] files = dir.GetFiles();
            foreach (FileInfo file in files)
            {
                string temppath = Path.Combine(destination, file.Name);
                file.CopyTo(temppath, false);
            }

            foreach (DirectoryInfo subdir in dirs)
            {
                string temppath = Path.Combine(destination, subdir.Name);
                Copy(subdir.FullName, temppath);
            }
        }
    }
}
