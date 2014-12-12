using System;
using System.Collections.Generic;
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

    public enum HandlePositionMode
    {
        Center,
        Pivot
    }

    public enum HandleCoordinateMode
    {
        Local,
        World
    }

    public class EditorApplication
    {
        private static SceneViewTool activeSceneTool = SceneViewTool.Move; // TODO - Actually retrieve this from somewhere
        private static HandlePositionMode handlePositionMode = HandlePositionMode.Pivot; // TODO - Actually retrieve this from somewhere
        private static HandleCoordinateMode handleCoordinateMode = HandleCoordinateMode.World; // TODO - Actually retrieve this from somewhere

        public static SceneViewTool ActiveSceneTool
        {
            get { return activeSceneTool; }
            set { activeSceneTool = value; } // TODO - Will likely need to update active GUI button when this changes
        }

        public static HandlePositionMode HandlePositionMode
        {
            get { return handlePositionMode; }
            set { handlePositionMode = value; } // TODO - Will likely need to update active GUI button when this changes
        }

        public static HandleCoordinateMode HandleCoordinateMode
        {
            get { return handleCoordinateMode; }
            set { handleCoordinateMode = value; } // TODO - Will likely need to update active GUI button when this changes
        }

        public static Camera SceneViewCamera
        {
            get { return instance.scene.GetCamera(); }
        }

        private static EditorApplication instance;

        private InspectorWindow inspector;
        private SceneWindow scene;

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

            // DEBUG ONLY

            SceneObject newDbgObject = new SceneObject("NewDbgObject");
            dbgComponent = newDbgObject.AddComponent<Debug_Component1>();
            newDbgObject.AddComponent<Debug_Component2>();

            inspector.SetObjectToInspect(newDbgObject);

            SceneObject gizmoDbgObject = new SceneObject("GizmoDebug");
            gizmoDbgObject.AddComponent<DbgGizmoComponent>();

            // DEBUG ONLY END
        }

        internal void EditorUpdate()
        {
            // DEBUG ONLY

            if (dbgComponent != null)
                dbgComponent.intArray[0] = dbgComponent.intArray[0] + 1;

            // DEBUG ONLY END
        }
    }
}
