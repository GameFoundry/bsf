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
                initialPosition = so.LocalPosition;
                initialRotation = so.LocalRotation;
                initialScale = so.LocalScale;
            }

            public SceneObject so;
            public Vector3 initialPosition;
            public Quaternion initialRotation;
            public Vector3 initialScale;
        }

        private SceneViewTool activeHandleType = SceneViewTool.View;
        private DefaultHandle activeHandle;

        private HandledObject[] activeSelection;
        private bool isDragged;
        private Quaternion initialHandleRotation;

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
                if (EditorApplication.ActiveCoordinateMode == HandleCoordinateMode.World)
                    rotation = Quaternion.identity;
                else
                    rotation = selectedSceneObjects[0].Rotation; // We don't average rotation in case of multi-selection

                Vector3 position;
                if (EditorApplication.ActivePivotMode == HandlePivotMode.Pivot)
                    position = selectedSceneObjects[0].Position; // Just take pivot from the first one, no averaging
                else
                {
                    List<SceneObject> flatenedHierarchy = new List<SceneObject>();
                    foreach (var so in selectedSceneObjects)
                        flatenedHierarchy.AddRange(EditorUtility.FlattenHierarchy(so));

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

                        initialHandleRotation = activeHandle.Rotation;
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
                            MoveHandle moveHandle = (MoveHandle) activeHandle;

                            foreach (var selectedObj in activeSelection)
                                selectedObj.so.LocalPosition = selectedObj.initialPosition + moveHandle.Delta;

                            break;
                        case SceneViewTool.Rotate:
                        {
                            RotateHandle rotateHandle = (RotateHandle) activeHandle;

                            // Make sure we transform relative to the handle position
                            SceneObject temporarySO = new SceneObject("Temp");
                            temporarySO.Position = activeHandle.Position;
                            temporarySO.LocalRotation = initialHandleRotation;

                            SceneObject[] originalParents = new SceneObject[activeSelection.Length];
                            for (int i = 0; i < activeSelection.Length; i++)
                            {
                                originalParents[i] = activeSelection[i].so.Parent;
                                activeSelection[i].so.LocalPosition = activeSelection[i].initialPosition;
                                activeSelection[i].so.LocalRotation = activeSelection[i].initialRotation;
                                activeSelection[i].so.Parent = temporarySO;
                            }

                            temporarySO.LocalRotation *= rotateHandle.Delta;

                            for (int i = 0; i < activeSelection.Length; i++)
                                activeSelection[i].so.Parent = originalParents[i];

                            temporarySO.Destroy();
                        }
                            break;
                        case SceneViewTool.Scale:
                        {
                            ScaleHandle scaleHandle = (ScaleHandle) activeHandle;

                            // Make sure we transform relative to the handle position
                            SceneObject temporarySO = new SceneObject("Temp");
                            temporarySO.Position = activeHandle.Position;

                            SceneObject[] originalParents = new SceneObject[activeSelection.Length];
                            for (int i = 0; i < activeSelection.Length; i++)
                            {
                                originalParents[i] = activeSelection[i].so.Parent;
                                activeSelection[i].so.LocalScale = activeSelection[i].initialScale;
                                activeSelection[i].so.Parent = temporarySO;
                            }

                            //temporarySO.LocalScale = Vector3.one + scaleHandle.Delta;
                            temporarySO.LocalScale = Vector3.one*0.5f;
                            Debug.Log("SCALE " + temporarySO.LocalScale + " - " + scaleHandle.Delta);

                            for (int i = 0; i < activeSelection.Length; i++)
                            {
                                Debug.Log("POSITION A: " + activeSelection[i].so.Position);
                                activeSelection[i].so.Parent = originalParents[i];
                                Debug.Log("TFRM " + activeSelection[i].so.LocalScale + " - " +
                                          activeSelection[i].so.Position);
                            }

                            temporarySO.Destroy();
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
