using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handles operations with the active scene (level).
    /// </summary>
    public static class Scene
    {
        /// <summary>
        /// Returns the name of the scene prefab. This is empty if scene hasn't been saved yet.
        /// </summary>
        public static string ActiveSceneName { get { return activeSceneName; } }

        /// <summary>
        /// Returns the UUID of the scene prefab. This is empty if scene hasn't been saved yet.
        /// </summary>
        internal static string ActiveSceneUUID { get { return activeSceneUUID; } }

        private static string activeSceneName = "Unnamed";
        private static string activeSceneUUID = "";

        /// <summary>
        /// Returns the root scene object for the current scene.
        /// </summary>
        public static SceneObject Root
        {
            get { return Internal_GetRoot(); }
        }

        /// <summary>
        /// Returns the main camera that controls the final render surface that is displayed to the user. If the current
        /// scene has no main camera null is returned.
        /// </summary>
        public static Camera Camera
        {
            get
            {
                SceneObject so = Internal_GetMainCameraSO();

                if (so == null)
                    return null;

                return so.GetComponent<Camera>();
            }
        }

        /// <summary>
        /// Clears all scene objects from the current scene.
        /// </summary>
        public static void Clear()
        {
            Internal_ClearScene();
            activeSceneUUID = null;
            activeSceneName = "Unnamed";
        }

        /// <summary>
        /// Loads a new scene.
        /// </summary>
        /// <param name="path">Path to the prefab to load.</param>
        public static void Load(string path)
        {
            Clear();
            Prefab scene = Internal_LoadScene(path);

            SetActive(scene);
        }

        /// <summary>
        /// Sets the currently active scene to the provided scene.
        /// </summary>
        /// <param name="scene">Scene which to set as active.</param>
        internal static void SetActive(Prefab scene)
        {
            if (scene != null)
            {
                activeSceneUUID = scene.UUID;
                activeSceneName = scene.Name;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Prefab Internal_LoadScene(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetRoot();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearScene();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetMainCameraSO();
    }
}
