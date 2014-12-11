using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class DefaultHandleManager : Handle
    {
        private struct HandledObject
        {
            public HandledObject(SceneObject so)
            {
                this.so = so;
                initialPosition = so.position;
            }

            public SceneObject so;
            public Vector3 initialPosition;
        }

        private SceneViewTool activeHandleType = SceneViewTool.View;
        private DefaultHandle activeHandle;

        private HandledObject[] activeSelection;
        private bool isDragged;

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
                    rotation = selectedSceneObjects[0].rotation; // We don't average rotation in case of multi-selection

                Vector3 position;
                if (EditorApplication.HandlePositionMode == HandlePositionMode.Pivot)
                    position = selectedSceneObjects[0].position; // Just take pivot from the first one, no averaging
                else
                {
                    List<SceneObject> flatenedHierarchy = new List<SceneObject>();
                    foreach (var so in selectedSceneObjects)
                    {
                        flatenedHierarchy.AddRange(EditorUtility.FlattenHierarchy(so));
                    }

                    AABox selectionBounds = EditorUtility.CalculateBounds(flatenedHierarchy.ToArray());
                    position = selectionBounds.center;
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
                if (activeHandle.IsDragged())
                {
                    if (!isDragged)
                    {
                        isDragged = true;

                        SceneObject[] selectedSceneObjects = Selection.sceneObjects;
                        activeSelection = new HandledObject[selectedSceneObjects.Length];
                        for (int i = 0; i < selectedSceneObjects.Length; i++)
                            activeSelection[i] = new HandledObject(selectedSceneObjects[0]);
                    }
                }
                else
                {
                    isDragged = false;
                    activeSelection = null;
                }

                activeHandle.DoPostInput();

                if (activeHandle.IsDragged())
                {
                    switch (activeHandleType)
                    {
                        case SceneViewTool.Move:
                        {
                            MoveHandle moveHandle = (MoveHandle) activeHandle;

                            foreach (var selectedObj in activeSelection)
                                selectedObj.so.position = selectedObj.initialPosition + moveHandle.Delta;
                        }

                            break;
                        case SceneViewTool.Rotate:
                        {
                            RotateHandle rotateHandle = (RotateHandle) activeHandle;

                            // TODO - Add delta rotation
                            //foreach (var so in selectedSceneObjects)
                            //    so.rotation += rotateHandle.Delta;
                        }
                            break;
                        case SceneViewTool.Scale:
                        {
                            ScaleHandle scaleHandle = (ScaleHandle) activeHandle;

                            // TODO - Add delta scale
                            //foreach (var so in selectedSceneObjects)
                            //    so.localScale += scaleHandle.Delta;
                        }
                            break;
                    }
                }
            }
            else
            {
                isDragged = false;
                activeSelection = null;
            }
        }

        protected override void Draw()
        {
            if (activeHandle != null)
                activeHandle.DoDraw();
        }
    }
}
