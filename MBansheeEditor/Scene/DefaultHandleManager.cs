using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class DefaultHandleManager : Handle
    {
        private SceneViewTool activeHandleType = SceneViewTool.View;
        private DefaultHandle activeHandle;

        protected override void PreInput()
        {
            SceneObject[] selectedSceneObjects = Selection.sceneObjects;

            if (selectedSceneObjects.Length == 0)
            {
                if (activeHandle != null)
                {
                    activeHandle.Destroy();
                    activeHandle = null;
                }
            }
            else
            {
                if (activeHandleType != EditorApplication.ActiveSceneTool || activeHandle == null)
                {
                    if (activeHandle != null)
                    {
                        activeHandle.Destroy();
                        activeHandle = null;
                    }

                    switch (EditorApplication.ActiveSceneTool)
                    {
                        case SceneViewTool.Move:
                            activeHandle = new MoveHandle();
                            break;
                        case SceneViewTool.Rotate:
                            activeHandle = new RotateHandle();
                            break;
                        case SceneViewTool.Scale:
                            activeHandle = new ScaleHandle();
                            break;
                    }

                    activeHandleType = EditorApplication.ActiveSceneTool;
                }
            }

            if (activeHandle != null)
            {
                Quaternion rotation;
                if (EditorApplication.HandleCoordinateMode == HandleCoordinateMode.World)
                    rotation = Quaternion.identity;
                else
                    rotation = selectedSceneObjects[0].Rotation; // We don't average rotation in case of multi-selection

                Vector3 position;
                if (EditorApplication.HandlePositionMode == HandlePositionMode.Pivot)
                    position = selectedSceneObjects[0].Position; // Just take pivot from the first one, no averaging
                else
                {
                    List<SceneObject> flatenedHierarchy = new List<SceneObject>();
                    foreach (var so in selectedSceneObjects)
                    {
                        flatenedHierarchy.AddRange(EditorUtility.FlattenHierarchy(so));
                    }

                    AABox selectionBounds = EditorUtility.CalculateBounds(flatenedHierarchy.ToArray());
                    position = selectionBounds.Center;
                }

                activeHandle.Position = position;
                activeHandle.Rotation = rotation;

                activeHandle.DoPreInput();
            }
        }

        protected override void PostInput()
        {
            if (activeHandle != null)
            {
                activeHandle.DoPostInput();

                SceneObject[] selectedSceneObjects = Selection.sceneObjects;
                switch (activeHandleType)
                {
                    case SceneViewTool.Move:
                        {
                            MoveHandle moveHandle = (MoveHandle)activeHandle;

                            foreach (var so in selectedSceneObjects)
                                so.Position += moveHandle.Delta;
                        }

                        break;
                    case SceneViewTool.Rotate:
                        {
                            RotateHandle rotateHandle = (RotateHandle)activeHandle;

                            // TODO - Add delta rotation
                            //foreach (var so in selectedSceneObjects)
                            //    so.rotation += rotateHandle.Delta;
                        }
                        break;
                    case SceneViewTool.Scale:
                        {
                            ScaleHandle scaleHandle = (ScaleHandle)activeHandle;

                            // TODO - Add delta scale
                            //foreach (var so in selectedSceneObjects)
                            //    so.localScale += scaleHandle.Delta;
                        }
                        break;
                }
            }
        }

        protected override void Draw()
        {
            if (activeHandle != null)
                activeHandle.DoDraw();
        }
    }
}
