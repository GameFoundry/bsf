using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public static class Scene
    {
        internal static string ActiveSceneUUID { get; set; }

        public static bool IsModified()
        {
            // TODO - Needs implementing
            return true;
        }

        public static void Clear()
        {
            Internal_ClearScene();
            ActiveSceneUUID = null;
        }

        public static void Load(string path)
        {
            Clear();
            ActiveSceneUUID = Internal_LoadScene(path);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_LoadScene(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearScene();
    }
}
