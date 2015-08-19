using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public enum SceneViewTool
    {
        View,
        Move,
        Rotate,
        Scale
    }

    public enum HandlePivotMode
    {
        Center,
        Pivot
    }

    public enum HandleCoordinateMode
    {
        Local,
        World
    }

    public enum EditorPlatformType
    {
        Windows
    }

    public class EditorApplication
    {
        public static SceneViewTool ActiveSceneTool
        {
            get { return EditorSettings.ActiveSceneTool; }
            set { EditorSettings.ActiveSceneTool = value; }
        }

        public static HandleCoordinateMode ActiveCoordinateMode
        {
            get { return EditorSettings.ActiveCoordinateMode; }
            set { EditorSettings.ActiveCoordinateMode = value; }
        }

        public static HandlePivotMode ActivePivotMode
        {
            get { return EditorSettings.ActivePivotMode; }
            set { EditorSettings.ActivePivotMode = value; }
        }

        public static Camera SceneViewCamera
        {
            get { return EditorWindow.GetWindow<SceneWindow>().GetCamera(); }
        }

        public static EditorPlatformType EditorPlatform
        {
            get { return EditorPlatformType.Windows; } // TODO - Set this properly once we have support for more platforms
        }

        public static string ProjectPath { get { return Internal_GetProjectPath(); } }
        public static string ProjectName { get { return Internal_GetProjectName(); } }
        internal static string CompilerPath { get { return Internal_GetCompilerPath(); } }
        internal static string BuiltinAssemblyPath { get { return Internal_GetBuiltinAssemblyPath(); } }
        internal static string ScriptAssemblyPath { get { return Internal_GetScriptAssemblyPath(); } }
        internal static string FrameworkAssemblyPath { get { return Internal_GetFrameworkAssemblyPath(); } }
        internal static string EngineAssembly { get { return Internal_GetEngineAssemblyName(); } }
        internal static string EditorAssembly { get { return Internal_GetEditorAssemblyName(); } }
        internal static string ScriptGameAssembly { get { return Internal_GetScriptGameAssemblyName(); } }
        internal static string ScriptEditorAssembly { get { return Internal_GetScriptEditorAssemblyName(); } }

        private static EditorApplication instance;

        private FolderMonitor monitor;

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

            ProjectLibrary.Refresh();
            monitor = new FolderMonitor(ProjectLibrary.ResourceFolder);
            monitor.OnAdded += OnAssetModified;
            monitor.OnRemoved += OnAssetModified;
            monitor.OnModified += OnAssetModified;
        }

        private void OnAssetModified(string path)
        {
            ProjectLibrary.Refresh(path);
        }

        internal void OnEditorUpdate()
        {
            ProjectLibrary.Update();
        }

        [MenuItem("File/Save Prefab", ButtonModifier.Ctrl, ButtonCode.S, 50, true)]
        private static void SavePrefab()
        {
            if (!string.IsNullOrEmpty(Scene.ActiveSceneUUID))
            {
                string scenePath = ProjectLibrary.GetPath(Scene.ActiveSceneUUID);
                Internal_SaveScene(scenePath);
            }
            else
                SavePrefabAs();
        }

        [MenuItem("File/Save Prefab As", 50)]
        private static void SavePrefabAs()
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

        [MenuItem("File/Load Prefab", ButtonModifier.Ctrl, ButtonCode.L, 50)]
        private static void LoadPrefab()
        {
            Action doLoad =
                () =>
                {
                    string[] scenePaths;
                    if (BrowseDialog.OpenFile(ProjectLibrary.ResourceFolder, "", false, out scenePaths))
                    {
                        if (scenePaths.Length > 0)
                            Scene.Load(scenePaths[0]);
                    }
                };

            Action<DialogBox.ResultType> dialogCallback =
                (result) =>
                {
                    if (result == DialogBox.ResultType.Yes)
                    {
                        SavePrefab();
                        doLoad();
                    }
                    else if (result == DialogBox.ResultType.No)
                        doLoad();
                };

            if (Scene.IsModified())
            {
                DialogBox.Open("Warning", "You current scene has modifications. Do you wish to save them first?",
                    DialogBox.Type.YesNoCancel, dialogCallback);
            }
            else
                doLoad();
        }

        [MenuItem("Components/Camera")]
        private static void AddCamera()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Camera component");
            so.AddComponent<Camera>();
        }

        [MenuItem("Components/Renderable")]
        private static void AddRenderable()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Renderable component");
            so.AddComponent<Renderable>();
        }

        [MenuItem("Components/Point light")]
        private static void AddPointLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;
        }

        [MenuItem("Components/Spot light")]
        private static void AddSpotLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;
        }

        [MenuItem("Components/Directional light")]
        private static void AddDirectionalLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;
        }

        [MenuItem("Scene Objects/Camera")]
        private static void AddCameraSO()
        {
            SceneObject so = UndoRedo.CreateSO("Camera", "Created a Camera");
            so.AddComponent<Camera>();

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Renderable")]
        private static void AddRenderableSO()
        {
            SceneObject so = UndoRedo.CreateSO("Renderable", "Created a Renderable");
            so.AddComponent<Renderable>();

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Point light")]
        private static void AddPointLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Point light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Spot light")]
        private static void AddSpotLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Spot light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Directional light")]
        private static void AddDirectionalLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Directional light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;

            Selection.sceneObject = so;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetProjectPath();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetProjectName();

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
    }
}
