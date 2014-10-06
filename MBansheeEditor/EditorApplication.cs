using System;
using System.Collections.Generic;

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
    }
}
