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
            get { return instance.scene.GetCamera(); }
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

        private InspectorWindow inspector;
        private SceneWindow scene;
        private DebugWindow debug;

        private FolderMonitor monitor;

        // DEBUG ONLY
        Debug_Component1 dbgComponent;
        // END DEBUG ONLY

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
            inputConfig.RegisterButton(SceneCamera.MoveBackBinding, ButtonCode.Back);
            inputConfig.RegisterButton(SceneCamera.MoveLeftBinding, ButtonCode.Left);
            inputConfig.RegisterButton(SceneCamera.MoveRightBinding, ButtonCode.Right);
            inputConfig.RegisterButton(SceneCamera.FastMoveBinding, ButtonCode.LeftShift);
            inputConfig.RegisterButton(SceneCamera.RotateBinding, ButtonCode.MouseRight);
            inputConfig.RegisterAxis(SceneCamera.HorizontalAxisBinding, InputAxis.MouseX);
            inputConfig.RegisterAxis(SceneCamera.VerticalAxisBinding, InputAxis.MouseY);

            // Open windows
            inspector = EditorWindow.OpenWindow<InspectorWindow>();
            scene = EditorWindow.OpenWindow<SceneWindow>();
            debug = EditorWindow.OpenWindow<DebugWindow>();

            ProjectLibrary.Refresh();
            monitor = new FolderMonitor(ProjectLibrary.ResourceFolder);
            monitor.OnAdded += OnAssetModified;
            monitor.OnRemoved += OnAssetModified;
            monitor.OnModified += OnAssetModified;

            // DEBUG ONLY

            SceneObject newDbgObject = new SceneObject("NewDbgObject");
            dbgComponent = newDbgObject.AddComponent<Debug_Component1>();
            newDbgObject.AddComponent<Debug_Component2>();

            inspector.SetObjectToInspect(newDbgObject);

            SceneObject gizmoDbgObject = new SceneObject("GizmoDebug");
            gizmoDbgObject.AddComponent<DbgGizmoComponent>();

            //ProgressBar.Show("Test", 0.5f);
            //ColorPicker.Show();

            // DEBUG ONLY END
        }

        private void OnAssetModified(string path)
        {
            ProjectLibrary.Refresh(path);
        }

        internal void OnEditorUpdate()
        {
            ProjectLibrary.Update();

            // DEBUG ONLY

            if (dbgComponent != null)
                dbgComponent.intArray[0] = dbgComponent.intArray[0] + 1;

            // DEBUG ONLY END
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
    }
}
