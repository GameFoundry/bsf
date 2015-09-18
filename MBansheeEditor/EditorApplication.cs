using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.IO;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Available tools in the scene view.
    /// </summary>
    public enum SceneViewTool
    {
        View,
        Move,
        Rotate,
        Scale
    }

    /// <summary>
    /// Pivot mode used by the scene view tools.
    /// </summary>
    public enum HandlePivotMode
    {
        Center,
        Pivot
    }

    /// <summary>
    /// Coordinate mode used by the scene view tools.
    /// </summary>
    public enum HandleCoordinateMode
    {
        Local,
        World
    }

    /// <summary>
    /// Manages various generic and global settings relating to the editor.
    /// </summary>
    public class EditorApplication
    {
        /// <summary>
        /// Determines the active tool shown in the scene view.
        /// </summary>
        public static SceneViewTool ActiveSceneTool
        {
            get { return EditorSettings.ActiveSceneTool; }
            set { EditorSettings.ActiveSceneTool = value; }
        }

        /// <summary>
        /// Determines the coordinate mode used by the tools in the scene view.
        /// </summary>
        public static HandleCoordinateMode ActiveCoordinateMode
        {
            get { return EditorSettings.ActiveCoordinateMode; }
            set { EditorSettings.ActiveCoordinateMode = value; }
        }

        /// <summary>
        /// Determines the pivot mode used by the tools in the scene view.
        /// </summary>
        public static HandlePivotMode ActivePivotMode
        {
            get { return EditorSettings.ActivePivotMode; }
            set { EditorSettings.ActivePivotMode = value; }
        }

        /// <summary>
        /// Camera used for rendering the scene view.
        /// </summary>
        public static Camera SceneViewCamera
        {
            get { return EditorWindow.GetWindow<SceneWindow>().Camera; }
        }

        /// <summary>
        /// Absolute path to the folder containing the currently open project.
        /// </summary>
        public static string ProjectPath { get { return Internal_GetProjectPath(); } }

        /// <summary>
        /// Name of the currently open project.
        /// </summary>
        public static string ProjectName { get { return Internal_GetProjectName(); } }

        /// <summary>
        /// Checks is any project currently loaded.
        /// </summary>
        public static bool IsProjectLoaded { get { return Internal_GetProjectLoaded(); } }

        /// <summary>
        /// Returns the path where the script compiler is located at.
        /// </summary>
        internal static string CompilerPath { get { return Internal_GetCompilerPath(); } }

        /// <summary>
        /// Returns the path to the folder where the builtin script assemblies are located at.
        /// </summary>
        internal static string BuiltinAssemblyPath { get { return Internal_GetBuiltinAssemblyPath(); } }

        /// <summary>
        /// Returns the path to the folder where the custom script assemblies are located at.
        /// </summary>
        internal static string ScriptAssemblyPath { get { return Internal_GetScriptAssemblyPath(); } }

        /// <summary>
        /// Returns the path to the folder where the .NET framework assemblies are located at.
        /// </summary>
        internal static string FrameworkAssemblyPath { get { return Internal_GetFrameworkAssemblyPath(); } }

        /// <summary>
        /// Name of the builtin assembly containing engine specific types.
        /// </summary>
        internal static string EngineAssembly { get { return Internal_GetEngineAssemblyName(); } }

        /// <summary>
        /// Name of the builtin assembly containing editor specific types.
        /// </summary>
        internal static string EditorAssembly { get { return Internal_GetEditorAssemblyName(); } }

        /// <summary>
        /// Name of the custom assembly compiled from non-editor scripts within the project.
        /// </summary>
        internal static string ScriptGameAssembly { get { return Internal_GetScriptGameAssemblyName(); } }

        /// <summary>
        /// Name of the custom assembly compiled from editor scripts within the project.
        /// </summary>
        internal static string ScriptEditorAssembly { get { return Internal_GetScriptEditorAssemblyName(); } }

        private static EditorApplication instance;
        private static FolderMonitor monitor;

        /// <summary>
        /// Constructs a new editor application. Called at editor start-up by the runtime.
        /// </summary>
        internal EditorApplication()
        {
            instance = this;

            // Register controls
            InputConfiguration inputConfig = VirtualInput.KeyConfig;

            inputConfig.RegisterButton(SceneCamera.MoveForwardBinding, ButtonCode.W);
            inputConfig.RegisterButton(SceneCamera.MoveBackBinding, ButtonCode.S);
            inputConfig.RegisterButton(SceneCamera.MoveLeftBinding, ButtonCode.A);
            inputConfig.RegisterButton(SceneCamera.MoveRightBinding, ButtonCode.D);
            inputConfig.RegisterButton(SceneCamera.MoveForwardBinding, ButtonCode.Up);
            inputConfig.RegisterButton(SceneCamera.MoveBackBinding, ButtonCode.Down);
            inputConfig.RegisterButton(SceneCamera.MoveLeftBinding, ButtonCode.Left);
            inputConfig.RegisterButton(SceneCamera.MoveRightBinding, ButtonCode.Right);
            inputConfig.RegisterButton(SceneCamera.FastMoveBinding, ButtonCode.LeftShift);
            inputConfig.RegisterButton(SceneCamera.RotateBinding, ButtonCode.MouseRight);
            inputConfig.RegisterAxis(SceneCamera.HorizontalAxisBinding, InputAxis.MouseX);
            inputConfig.RegisterAxis(SceneCamera.VerticalAxisBinding, InputAxis.MouseY);

            inputConfig.RegisterButton(SceneWindow.ToggleProfilerOverlayBinding, ButtonCode.P, ButtonModifier.CtrlAlt);
            inputConfig.RegisterButton(SceneWindow.ViewToolBinding, ButtonCode.Q);
            inputConfig.RegisterButton(SceneWindow.MoveToolBinding, ButtonCode.W);
            inputConfig.RegisterButton(SceneWindow.RotateToolBinding, ButtonCode.E);
            inputConfig.RegisterButton(SceneWindow.ScaleToolBinding, ButtonCode.R);
            inputConfig.RegisterButton(SceneWindow.DuplicateBinding, ButtonCode.D, ButtonModifier.Ctrl);
        }

        /// <summary>
        /// Triggered when the folder monitor detects an asset in the monitored folder was modified.
        /// </summary>
        /// <param name="path">Path to the modified file or folder.</param>
        private static void OnAssetModified(string path)
        {
            ProjectLibrary.Refresh(path);
        }

        /// <summary>
        /// Called 60 times per second by the runtime.
        /// </summary>
        internal void OnEditorUpdate()
        {
            ProjectLibrary.Update();
        }

        /// <summary>
        /// Opens a dialog that allows the user to select a new prefab to load as the current scene. If current scene
        /// is modified the user is offered a chance to save it.
        /// </summary>
        [MenuItem("File/Open Scene", ButtonModifier.Ctrl, ButtonCode.L, 10050, true)]
        private static void LoadScene()
        {
            string[] scenePaths;
            if (BrowseDialog.OpenFile(ProjectLibrary.ResourceFolder, "", false, out scenePaths))
            {
                if (scenePaths.Length > 0)
                    LoadScene(scenePaths[0]);
            }
        }

        /// <summary>
        /// Opens a dialog to allows the user to select a location where to save the current scene. If scene was previously
        /// saved it is instead automatically saved at the last location.
        /// </summary>
        [MenuItem("File/Save Scene", ButtonModifier.Ctrl, ButtonCode.S, 10049)]
        private static void SaveScene()
        {
            if (!string.IsNullOrEmpty(Scene.ActiveSceneUUID))
            {
                string scenePath = ProjectLibrary.GetPath(Scene.ActiveSceneUUID);
                Internal_SaveScene(scenePath);
            }
            else
                SaveSceneAs();
        }

        /// <summary>
        /// Opens a dialog to allows the user to select a location where to save the current scene.
        /// </summary>
        [MenuItem("File/Save Scene As", 10048)]
        private static void SaveSceneAs()
        {
            string scenePath = "";
            if (BrowseDialog.SaveFile(ProjectLibrary.ResourceFolder, "*.prefab", out scenePath))
            {
                if (!PathEx.IsPartOf(scenePath, ProjectLibrary.ResourceFolder))
                    DialogBox.Open("Error", "The location must be inside the Resources folder of the project.",
                        DialogBox.Type.OK);
                else
                {
                    // TODO - If path points to an existing non-scene asset or folder I should delete it otherwise
                    //        Internal_SaveScene will silently fail.

                    Scene.ActiveSceneUUID = Internal_SaveScene(scenePath + ".prefab");
                }
            }
        }

        /// <summary>
        /// Loads a prefab as the current scene at the specified path. If current scene is modified the user is offered a 
        /// chance to save it.
        /// </summary>
        /// <param name="path">Path to a valid prefab, relative to the resource folder.</param>
        public static void LoadScene(string path)
        {
            Action<string> continueLoad =
                (scenePath) =>
                {
                    Scene.Load(path);

                    ProjectSettings.LastOpenScene = scenePath;
                    ProjectSettings.Save();
                };

            Action<DialogBox.ResultType> dialogCallback =
            (result) =>
            {
                if (result == DialogBox.ResultType.Yes)
                {
                    SaveScene();
                    continueLoad(path);
                }
                else if (result == DialogBox.ResultType.No)
                    continueLoad(path);
            };

            if (Scene.IsModified())
            {
                DialogBox.Open("Warning", "You current scene has modifications. Do you wish to save them first?",
                    DialogBox.Type.YesNoCancel, dialogCallback);
            }
            else
                continueLoad(path);
        }

        /// <summary>
        /// Checks does the folder at the provieded path contain a valid project.
        /// </summary>
        /// <param name="path">Absolute path to the root project folder.</param>
        /// <returns>True if the folder contains a valid project.</returns>
        public static bool IsValidProject(string path)
        {
            return Internal_IsValidProject(path);
        }

        /// <summary>
        /// Contains a new project in the provided folder.
        /// </summary>
        /// <param name="path">Absolute path to the folder to create the project in. Name of this folder will be used as the
        ///                    project's name.</param>
        public static void CreateProject(string path)
        {
            Internal_CreateProject(path);
        }

        /// <summary>
        /// Opens a Project Window allowing you to browse for or create a project.
        /// </summary>
        [MenuItem("File/Open Project", 10100)]
        public static void BrowseForProject()
        {
            ProjectWindow.Open();
        }

        /// <summary>
        /// Saves all data in the currently open project.
        /// </summary>
        [MenuItem("File/Save Project", 10099)]
        public static void SaveProject()
        {
            // TODO - Save dirty resources

            Internal_SaveProject();
        }

        /// <summary>
        /// Loads the project at the specified path. This method executes asynchronously and will trigger 
        /// <see cref="OnProjectLoaded"/> when done.
        /// </summary>
        /// <param name="path">Absolute path to the project's root folder.</param>
        public static void LoadProject(string path)
        {
            if (IsProjectLoaded && path == ProjectPath)
                return;

            if (!Internal_IsValidProject(path))
            {
                Debug.LogWarning("Provided path: \"" + path + "\" is not a valid project.");
                return;
            }

            if (IsProjectLoaded)
                UnloadProject();

            Internal_LoadProject(path); // Triggers OnProjectLoaded when done
        }

        /// <summary>
        /// Opens a file or a folder in the default external application.
        /// </summary>
        /// <param name="path">Absolute path to the file or folder to open.</param>
        public static void OpenExternally(string path)
        {
            Internal_OpenExternally(path);
        }

        /// <summary>
        /// Triggered when <see cref="LoadProject"/> method completes.
        /// </summary>
        private static void OnProjectLoaded()
        {
            if (!IsProjectLoaded)
            {
                ProjectWindow.Open();
                return;
            }

            string projectPath = ProjectPath;

            RecentProject[] recentProjects = EditorSettings.RecentProjects;
            bool foundPath = false;
            for (int i = 0; i < recentProjects.Length; i++)
            {
                if (PathEx.Compare(recentProjects[i].path, projectPath))
                {
                    recentProjects[i].accessTimestamp = (ulong)DateTime.Now.Ticks;
                    EditorSettings.RecentProjects = recentProjects;
                    foundPath = true;
                    break;
                }
            }

            if (!foundPath)
            {
                List<RecentProject> extendedRecentProjects = new List<RecentProject>();
                extendedRecentProjects.AddRange(recentProjects);

                RecentProject newProject = new RecentProject();
                newProject.path = projectPath;
                newProject.accessTimestamp = (ulong)DateTime.Now.Ticks;

                extendedRecentProjects.Add(newProject);

                EditorSettings.RecentProjects = extendedRecentProjects.ToArray();
            }

            EditorSettings.LastOpenProject = projectPath;
            EditorSettings.Save();

            ProjectLibrary.Refresh();
            monitor = new FolderMonitor(ProjectLibrary.ResourceFolder);
            monitor.OnAdded += OnAssetModified;
            monitor.OnRemoved += OnAssetModified;
            monitor.OnModified += OnAssetModified;

            if (!string.IsNullOrWhiteSpace(ProjectSettings.LastOpenScene))
                Scene.Load(ProjectSettings.LastOpenScene);
        }

        /// <summary>
        /// Unloads the currently loaded project. Offers the user a chance to save the current scene if it is modified.
        /// Automatically saves all project data before unloading.
        /// </summary>
        private static void UnloadProject()
        {
            Action continueUnload =
                () =>
                {
                    Scene.Clear();

                    if (monitor != null)
                    {
                        monitor.Destroy();
                        monitor = null;
                    }

                    LibraryWindow window = EditorWindow.GetWindow<LibraryWindow>();
                    if(window != null)
                        window.Reset();

                    Internal_UnloadProject();
                };

            Action<DialogBox.ResultType> dialogCallback =
            (result) =>
            {
                if (result == DialogBox.ResultType.Yes)
                    SaveScene();

                continueUnload();
            };

            if (Scene.IsModified())
            {
                DialogBox.Open("Warning", "You current scene has modifications. Do you wish to save them first?",
                    DialogBox.Type.YesNoCancel, dialogCallback);
            }
            else
                continueUnload();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetProjectPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetProjectName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetProjectLoaded();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetCompilerPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetBuiltinAssemblyPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetScriptAssemblyPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetFrameworkAssemblyPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetEngineAssemblyName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetEditorAssemblyName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetScriptGameAssemblyName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetScriptEditorAssemblyName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_SaveScene(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsValidProject(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SaveProject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_LoadProject(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnloadProject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateProject(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_OpenExternally(string path);
    }
}
