using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays the scene view camera and various scene controls.
    /// </summary>
    internal sealed class SceneWindow : EditorWindow
    {
        internal const string ToggleProfilerOverlayBinding = "ToggleProfilerOverlay";
        internal const string ViewToolBinding = "ViewTool";
        internal const string MoveToolBinding = "MoveTool";
        internal const string RotateToolBinding = "RotateTool";
        internal const string ScaleToolBinding = "ScaleTool";
        internal const string DuplicateBinding = "Duplicate";
        internal const string DeleteBinding = "Delete";
        internal const string FrameBinding = "SceneFrame";

        private const int HeaderHeight = 20;
        private const float DefaultPlacementDepth = 5.0f;
        private static readonly Color ClearColor = new Color(83.0f/255.0f, 83.0f/255.0f, 83.0f/255.0f);
        private const string ProfilerOverlayActiveKey = "_Internal_ProfilerOverlayActive";
        private const int HandleAxesGUISize = 50;
        private const int HandleAxesGUIPadding = 5;

        private Camera camera;
        private SceneCamera cameraController;
        private RenderTexture2D renderTexture;
        private GUILayoutY mainLayout;
        private GUIPanel rtPanel;

        private GUIRenderTexture renderTextureGUI;
        private SceneGrid sceneGrid;
        private SceneSelection sceneSelection;
        private SceneGizmos sceneGizmos;
        private SceneHandles sceneHandles;

        private GUIToggle viewButton;
        private GUIToggle moveButton;
        private GUIToggle rotateButton;
        private GUIToggle scaleButton;

        private GUIToggle localCoordButton;
        private GUIToggle worldCoordButton;

        private GUIToggle pivotButton;
        private GUIToggle centerButton;

        private GUIToggle moveSnapButton;
        private GUIFloatField moveSnapInput;

        private GUIToggle rotateSnapButton;
        private GUIFloatField rotateSnapInput;

        private SceneAxesGUI sceneAxesGUI;

        private int editorSettingsHash = int.MaxValue;

        private VirtualButton duplicateKey;
        private VirtualButton deleteKey;
        private VirtualButton frameKey;

        // Tool shortcuts
        private VirtualButton viewToolKey;
        private VirtualButton moveToolKey;
        private VirtualButton rotateToolKey;
        private VirtualButton scaleToolKey;

        // Profiler overlay
        private ProfilerOverlay activeProfilerOverlay;
        private Camera profilerCamera;
        private VirtualButton toggleProfilerOverlayKey;

        // Drag & drop
        private bool dragActive;
        private SceneObject draggedSO;

        /// <summary>
        /// Returns the scene camera.
        /// </summary>
        public Camera Camera
        {
            get { return camera; }
        }

        /// <summary>
        /// Constructs a new scene window.
        /// </summary>
        internal SceneWindow()
        { }

        /// <summary>
        /// Opens a scene window if its not open already.
        /// </summary>
        [MenuItem("Windows/Scene", ButtonModifier.CtrlAlt, ButtonCode.S, 6000)]
        private static void OpenSceneWindow()
        {
            OpenWindow<SceneWindow>();
        }

        /// <summary>
        /// Focuses on the currently selected object.
        /// </summary>
        [MenuItem("Tools/Frame Selected", ButtonModifier.None, ButtonCode.F, 9275, true)]
        private static void OpenSettingsWindow()
        {
            SceneWindow window = GetWindow<SceneWindow>();
            if (window != null)
                window.cameraController.FrameSelected();
        }

        /// <summary>
        /// Switches the active tool to the view tool.
        /// </summary>
        [MenuItem("Tools/View", ButtonModifier.Ctrl, ButtonCode.Q, 9274, true)]
        private static void SetViewTool()
        {
            SceneWindow window = GetWindow<SceneWindow>();
            if (window != null)
                window.OnSceneToolButtonClicked(SceneViewTool.View);
        }

        /// <summary>
        /// Switches the active tool to the move tool.
        /// </summary>
        [MenuItem("Tools/Move", ButtonModifier.Ctrl, ButtonCode.W, 9273)]
        private static void SetMoveTool()
        {
            SceneWindow window = GetWindow<SceneWindow>();
            if (window != null)
                window.OnSceneToolButtonClicked(SceneViewTool.Move);
        }

        /// <summary>
        /// Switches the active tool to the rotate tool.
        /// </summary>
        [MenuItem("Tools/Rotate", ButtonModifier.Ctrl, ButtonCode.E, 9272)]
        private static void SetRotateTool()
        {
            SceneWindow window = GetWindow<SceneWindow>();
            if (window != null)
                window.OnSceneToolButtonClicked(SceneViewTool.Rotate);
        }

        /// <summary>
        /// Switches the active tool to the scale tool.
        /// </summary>
        [MenuItem("Tools/Scale", ButtonModifier.Ctrl, ButtonCode.R, 9271)]
        private static void SetScaleTool()
        {
            SceneWindow window = GetWindow<SceneWindow>();
            if (window != null)
                window.OnSceneToolButtonClicked(SceneViewTool.Scale);
        }

        /// <inheritdoc/>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Scene");
        }

        private void OnInitialize()
        {
            mainLayout = GUI.AddLayoutY();

            GUIContent viewIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.View), 
                new LocEdString("View"));
            GUIContent moveIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Move), 
                new LocEdString("Move"));
            GUIContent rotateIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Rotate), 
                new LocEdString("Rotate"));
            GUIContent scaleIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Scale), 
                new LocEdString("Scale"));

            GUIContent localIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Local), 
                new LocEdString("Local"));
            GUIContent worldIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.World), 
                new LocEdString("World"));

            GUIContent pivotIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Pivot), 
                new LocEdString("Pivot"));
            GUIContent centerIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.Center), 
                new LocEdString("Center"));

            GUIContent moveSnapIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.MoveSnap), 
                new LocEdString("Move snap"));
            GUIContent rotateSnapIcon = new GUIContent(EditorBuiltin.GetSceneWindowIcon(SceneWindowIcon.RotateSnap), 
                new LocEdString("Rotate snap"));

            GUIToggleGroup handlesTG = new GUIToggleGroup();
            viewButton = new GUIToggle(viewIcon, handlesTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            moveButton = new GUIToggle(moveIcon, handlesTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            rotateButton = new GUIToggle(rotateIcon, handlesTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            scaleButton = new GUIToggle(scaleIcon, handlesTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));

            GUIToggleGroup coordModeTG = new GUIToggleGroup();
            localCoordButton = new GUIToggle(localIcon, coordModeTG, EditorStyles.Button, GUIOption.FlexibleWidth(75));
            worldCoordButton = new GUIToggle(worldIcon, coordModeTG, EditorStyles.Button, GUIOption.FlexibleWidth(75));

            GUIToggleGroup pivotModeTG = new GUIToggleGroup();
            pivotButton = new GUIToggle(pivotIcon, pivotModeTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            centerButton = new GUIToggle(centerIcon, pivotModeTG, EditorStyles.Button, GUIOption.FlexibleWidth(35));

            moveSnapButton = new GUIToggle(moveSnapIcon, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            moveSnapInput = new GUIFloatField("", GUIOption.FlexibleWidth(35));

            rotateSnapButton = new GUIToggle(rotateSnapIcon, EditorStyles.Button, GUIOption.FlexibleWidth(35));
            rotateSnapInput = new GUIFloatField("", GUIOption.FlexibleWidth(35));

            viewButton.OnClick += () => OnSceneToolButtonClicked(SceneViewTool.View);
            moveButton.OnClick += () => OnSceneToolButtonClicked(SceneViewTool.Move);
            rotateButton.OnClick += () => OnSceneToolButtonClicked(SceneViewTool.Rotate);
            scaleButton.OnClick += () => OnSceneToolButtonClicked(SceneViewTool.Scale);

            localCoordButton.OnClick += () => OnCoordinateModeButtonClicked(HandleCoordinateMode.Local);
            worldCoordButton.OnClick += () => OnCoordinateModeButtonClicked(HandleCoordinateMode.World);

            pivotButton.OnClick += () => OnPivotModeButtonClicked(HandlePivotMode.Pivot);
            centerButton.OnClick += () => OnPivotModeButtonClicked(HandlePivotMode.Center);

            moveSnapButton.OnToggled += (bool active) => OnMoveSnapToggled(active);
            moveSnapInput.OnChanged += (float value) => OnMoveSnapValueChanged(value);

            rotateSnapButton.OnToggled += (bool active) => OnRotateSnapToggled(active);
            rotateSnapInput.OnChanged += (float value) => OnRotateSnapValueChanged(value);

            GUILayout handlesLayout = mainLayout.AddLayoutX();
            handlesLayout.AddElement(viewButton);
            handlesLayout.AddElement(moveButton);
            handlesLayout.AddElement(rotateButton);
            handlesLayout.AddElement(scaleButton);
            handlesLayout.AddSpace(10);
            handlesLayout.AddElement(localCoordButton);
            handlesLayout.AddElement(worldCoordButton);
            handlesLayout.AddSpace(10);
            handlesLayout.AddElement(pivotButton);
            handlesLayout.AddElement(centerButton);
            handlesLayout.AddFlexibleSpace();
            handlesLayout.AddElement(moveSnapButton);
            handlesLayout.AddElement(moveSnapInput);
            handlesLayout.AddSpace(10);
            handlesLayout.AddElement(rotateSnapButton);
            handlesLayout.AddElement(rotateSnapInput);

            GUIPanel mainPanel = mainLayout.AddPanel();
            rtPanel = mainPanel.AddPanel();

            GUIPanel sceneAxesPanel = mainPanel.AddPanel(-1);
            sceneAxesGUI = new SceneAxesGUI(this, sceneAxesPanel, HandleAxesGUISize, HandleAxesGUISize);

            toggleProfilerOverlayKey = new VirtualButton(ToggleProfilerOverlayBinding);
            viewToolKey = new VirtualButton(ViewToolBinding);
            moveToolKey = new VirtualButton(MoveToolBinding);
            rotateToolKey = new VirtualButton(RotateToolBinding);
            scaleToolKey = new VirtualButton(ScaleToolBinding);
            duplicateKey = new VirtualButton(DuplicateBinding);
            deleteKey = new VirtualButton(DeleteBinding);
            frameKey = new VirtualButton(FrameBinding);

            UpdateRenderTexture(Width, Height - HeaderHeight);
            UpdateProfilerOverlay();
        }

        private void OnDestroy()
        {
            if (camera != null)
            {
                camera.SceneObject.Destroy();
                camera = null;
            }
        }
        
        /// <summary>
        /// Converts screen coordinates into coordinates relative to the scene view render texture.
        /// </summary>
        /// <param name="screenPos">Coordinates relative to the screen.</param>
        /// <param name="scenePos">Output coordinates relative to the scene view texture.</param>
        /// <returns>True if the coordinates are within the scene view texture, false otherwise.</returns>
        private bool ScreenToScenePos(Vector2I screenPos, out Vector2I scenePos)
        {
            scenePos = screenPos;
            Vector2I windowPos = ScreenToWindowPos(screenPos);

            Rect2I bounds = GUILayoutUtility.CalculateBounds(renderTextureGUI, GUI);
            if (bounds.Contains(windowPos))
            {
                scenePos.x = windowPos.x - bounds.x;
                scenePos.y = windowPos.y - bounds.y;

                return true;
            }

            return false;
        }

        private void OnEditorUpdate()
        {
            if (HasFocus)
            {
                if (!Input.IsPointerButtonHeld(PointerButton.Right))
                {
                    if (VirtualInput.IsButtonUp(toggleProfilerOverlayKey))
                        EditorSettings.SetBool(ProfilerOverlayActiveKey, !EditorSettings.GetBool(ProfilerOverlayActiveKey));

                    if (VirtualInput.IsButtonUp(viewToolKey))
                        EditorApplication.ActiveSceneTool = SceneViewTool.View;

                    if (VirtualInput.IsButtonUp(moveToolKey))
                        EditorApplication.ActiveSceneTool = SceneViewTool.Move;

                    if (VirtualInput.IsButtonUp(rotateToolKey))
                        EditorApplication.ActiveSceneTool = SceneViewTool.Rotate;

                    if (VirtualInput.IsButtonUp(scaleToolKey))
                        EditorApplication.ActiveSceneTool = SceneViewTool.Scale;

                    if (VirtualInput.IsButtonUp(duplicateKey))
                    {
                        SceneObject[] selectedObjects = Selection.SceneObjects;
                        CleanDuplicates(ref selectedObjects);

                        if (selectedObjects.Length > 0)
                        {
                            String message;
                            if (selectedObjects.Length == 1)
                                message = "Duplicated " + selectedObjects[0].Name;
                            else
                                message = "Duplicated " + selectedObjects.Length + " elements";

                            UndoRedo.CloneSO(selectedObjects, message);
                            EditorApplication.SetSceneDirty();
                        }
                    }

                    if (VirtualInput.IsButtonUp(deleteKey))
                    {
                        SceneObject[] selectedObjects = Selection.SceneObjects;
                        CleanDuplicates(ref selectedObjects);

                        if (selectedObjects.Length > 0)
                        {
                            foreach (var so in selectedObjects)
                            {
                                string message = "Deleted " + so.Name;
                                UndoRedo.DeleteSO(so, message);
                            }

                            EditorApplication.SetSceneDirty();
                        }
                    }
                }
            }

            // Refresh GUI buttons if needed (in case someones changes the values from script)
            if (editorSettingsHash != EditorSettings.Hash)
            {
                UpdateButtonStates();
                UpdateProfilerOverlay();
                editorSettingsHash = EditorSettings.Hash;
            }

            // Update scene view handles and selection
            sceneGizmos.Draw();
            sceneGrid.Draw();

            bool handleActive = false;
            if (Input.IsPointerButtonUp(PointerButton.Left))
            {
                if (sceneHandles.IsActive())
                {
                    sceneHandles.ClearSelection();
                    handleActive = true;
                }

                if (sceneAxesGUI.IsActive())
                {
                    sceneAxesGUI.ClearSelection();
                    handleActive = true;
                }
            }

            Vector2I scenePos;
            bool inBounds = ScreenToScenePos(Input.PointerPosition, out scenePos);

            bool draggedOver = DragDrop.DragInProgress || DragDrop.DropInProgress;
            draggedOver &= inBounds && DragDrop.Type == DragDropType.Resource;

            if (draggedOver)
            {
                if (DragDrop.DropInProgress)
                {
                    dragActive = false;

                    if (draggedSO != null)
                        Selection.SceneObject = draggedSO;

                    draggedSO = null;
                }
                else
                {
                    if (!dragActive)
                    {
                        dragActive = true;

                        ResourceDragDropData dragData = (ResourceDragDropData)DragDrop.Data;

                        string[] draggedPaths = dragData.Paths;

                        for (int i = 0; i < draggedPaths.Length; i++)
                        {
                            LibraryEntry entry = ProjectLibrary.GetEntry(draggedPaths[i]);
                            if (entry != null && entry.Type == LibraryEntryType.File)
                            {
                                FileEntry fileEntry = (FileEntry) entry;
                                if (fileEntry.ResType == ResourceType.Mesh)
                                {
                                    if (!string.IsNullOrEmpty(draggedPaths[i]))
                                    {
                                        string meshName = Path.GetFileNameWithoutExtension(draggedPaths[i]);
                                        draggedSO = UndoRedo.CreateSO(meshName, "Created a new Renderable \"" + meshName + "\"");
                                        Mesh mesh = ProjectLibrary.Load<Mesh>(draggedPaths[i]);

                                        Renderable renderable = draggedSO.AddComponent<Renderable>();
                                        renderable.Mesh = mesh;

                                        EditorApplication.SetSceneDirty();
                                    }

                                    break;
                                }
                                else if (fileEntry.ResType == ResourceType.Prefab)
                                {
                                    if (!string.IsNullOrEmpty(draggedPaths[i]))
                                    {
                                        Prefab prefab = ProjectLibrary.Load<Prefab>(draggedPaths[i]);
                                        draggedSO = UndoRedo.Instantiate(prefab, "Instantiating " + prefab.Name);

                                        EditorApplication.SetSceneDirty();
                                    }

                                    break;
                                }
                            }
                        }
                    }

                    if (draggedSO != null)
                    {
                        Ray worldRay = camera.ScreenToWorldRay(scenePos);
                        draggedSO.Position = worldRay*DefaultPlacementDepth;
                    }
                }

                return;
            }
            else
            {
                if (dragActive)
                {
                    dragActive = false;

                    if (draggedSO != null)
                    {
                        draggedSO.Destroy();
                        draggedSO = null;
                    }
                }
            }

            if (HasFocus)
            {
                cameraController.EnableInput(true);

                if (inBounds)
                {
                    if (Input.IsPointerButtonDown(PointerButton.Left))
                    {
                        Rect2I sceneAxesGUIBounds = new Rect2I(Width - HandleAxesGUISize - HandleAxesGUIPadding, 
                            HandleAxesGUIPadding, HandleAxesGUISize, HandleAxesGUISize);

                        if (sceneAxesGUIBounds.Contains(scenePos))
                            sceneAxesGUI.TrySelect(scenePos);
                        else
                            sceneHandles.TrySelect(scenePos);
                    }
                    else if (Input.IsPointerButtonUp(PointerButton.Left))
                    {
                        if (!handleActive)
                        {
                            bool ctrlHeld = Input.IsButtonHeld(ButtonCode.LeftControl) ||
                                            Input.IsButtonHeld(ButtonCode.RightControl);

                            sceneSelection.PickObject(scenePos, ctrlHeld);
                        }
                    }
                }
            }
            else
                cameraController.EnableInput(false);

            SceneHandles.BeginInput();
            sceneHandles.UpdateInput(scenePos, Input.PointerDelta);
            sceneHandles.Draw();

            sceneAxesGUI.UpdateInput(scenePos);
            sceneAxesGUI.Draw();
            SceneHandles.EndInput();

            sceneSelection.Draw();

            if (VirtualInput.IsButtonDown(frameKey))
            {
                cameraController.FrameSelected();
            }
        }

        /// <inheritdoc/>
        protected override void WindowResized(int width, int height)
        {
            UpdateRenderTexture(width, height - HeaderHeight);

            base.WindowResized(width, height);
        }

        /// <inheritdoc/>
        protected override void FocusChanged(bool inFocus)
        {
            if (!inFocus)
            {
                sceneHandles.ClearSelection();
            }
        }

        /// <summary>
        /// Triggered when one of the scene tool buttons is clicked, changing the active scene handle.
        /// </summary>
        /// <param name="tool">Clicked scene tool to activate.</param>
        private void OnSceneToolButtonClicked(SceneViewTool tool)
        {
            EditorApplication.ActiveSceneTool = tool;
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when one of the coordinate mode buttons is clicked, changing the active coordinate mode.
        /// </summary>
        /// <param name="mode">Clicked coordinate mode to activate.</param>
        private void OnCoordinateModeButtonClicked(HandleCoordinateMode mode)
        {
            EditorApplication.ActiveCoordinateMode = mode;
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when one of the pivot buttons is clicked, changing the active pivot mode.
        /// </summary>
        /// <param name="mode">Clicked pivot mode to activate.</param>
        private void OnPivotModeButtonClicked(HandlePivotMode mode)
        {
            EditorApplication.ActivePivotMode = mode;
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when the move snap button is toggled.
        /// </summary>
        /// <param name="active">Determins should be move snap be activated or deactivated.</param>
        private void OnMoveSnapToggled(bool active)
        {
            Handles.MoveHandleSnapActive = active;
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when the move snap increment value changes.
        /// </summary>
        /// <param name="value">Value that determines in what increments to perform move snapping.</param>
        private void OnMoveSnapValueChanged(float value)
        {
            Handles.MoveSnapAmount = MathEx.Clamp(value, 0.01f, 1000.0f);
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when the rotate snap button is toggled.
        /// </summary>
        /// <param name="active">Determins should be rotate snap be activated or deactivated.</param>
        private void OnRotateSnapToggled(bool active)
        {
            Handles.RotateHandleSnapActive = active;
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Triggered when the rotate snap increment value changes.
        /// </summary>
        /// <param name="value">Value that determines in what increments to perform rotate snapping.</param>
        private void OnRotateSnapValueChanged(float value)
        {
            Handles.RotateSnapAmount = MathEx.Clamp(value, 0.01f, 360.0f);
            editorSettingsHash = EditorSettings.Hash;
        }

        /// <summary>
        /// Updates toggle button states according to current editor options. This is useful if tools, coordinate mode,
        /// pivot or other scene view options have been modified externally.
        /// </summary>
        private void UpdateButtonStates()
        {
            switch (EditorApplication.ActiveSceneTool)
            {
                case SceneViewTool.View:
                    viewButton.Value = true;
                    break;
                case SceneViewTool.Move:
                    moveButton.Value = true;
                    break;
                case SceneViewTool.Rotate:
                    rotateButton.Value = true;
                    break;
                case SceneViewTool.Scale:
                    scaleButton.Value = true;
                    break;
            }

            switch (EditorApplication.ActiveCoordinateMode)
            {
                case HandleCoordinateMode.Local:
                    localCoordButton.Value = true;
                    break;
                case HandleCoordinateMode.World:
                    worldCoordButton.Value = true;
                    break;
            }

            switch (EditorApplication.ActivePivotMode)
            {
                case HandlePivotMode.Center:
                    centerButton.Value = true;
                    break;
                case HandlePivotMode.Pivot:
                    pivotButton.Value = true;
                    break;
            }

            if (Handles.MoveHandleSnapActive)
                moveSnapButton.Value = true;
            else
                moveSnapButton.Value = false;

            moveSnapInput.Value = Handles.MoveSnapAmount;

            if (Handles.RotateHandleSnapActive)
                rotateSnapButton.Value = true;
            else
                rotateSnapButton.Value = false;

            moveSnapInput.Value = Handles.RotateSnapAmount.Degrees;
        }

        /// <summary>
        /// Activates or deactivates the profiler overlay according to current editor settings.
        /// </summary>
        private void UpdateProfilerOverlay()
        {
            if (EditorSettings.GetBool(ProfilerOverlayActiveKey))
            {
                if (activeProfilerOverlay == null)
                {
                    SceneObject profilerSO = new SceneObject("EditorProfilerOverlay");
                    profilerCamera = profilerSO.AddComponent<Camera>();
                    profilerCamera.Target = renderTexture;
                    profilerCamera.ClearFlags = ClearFlags.None;
                    profilerCamera.Priority = 1;
                    profilerCamera.Layers = 0;

                    activeProfilerOverlay = profilerSO.AddComponent<ProfilerOverlay>();
                }
            }
            else
            {
                if (activeProfilerOverlay != null)
                {
                    activeProfilerOverlay.SceneObject.Destroy();
                    activeProfilerOverlay = null;
                    profilerCamera = null;
                }
            }
        }

        /// <summary>
        /// Creates the scene camera and updates the render texture. Should be called at least once before using the
        /// scene view. Should be called whenever the window is resized.
        /// </summary>
        /// <param name="width">Width of the scene render target, in pixels.</param>
        /// <param name="height">Height of the scene render target, in pixels.</param>
        private void UpdateRenderTexture(int width, int height)
	    {
            width = MathEx.Max(20, width);
            height = MathEx.Max(20, height);

            renderTexture = new RenderTexture2D(PixelFormat.R8G8B8A8, width, height);
            renderTexture.Priority = 1;

		    if (camera == null)
		    {
                SceneObject sceneCameraSO = new SceneObject("SceneCamera", true);
                camera = sceneCameraSO.AddComponent<Camera>();
                camera.Target = renderTexture;
                camera.ViewportRect = new Rect2(0.0f, 0.0f, 1.0f, 1.0f);

                sceneCameraSO.Position = new Vector3(0, 0.5f, 1);
                sceneCameraSO.LookAt(new Vector3(0, 0.5f, 0));

                camera.Priority = 2;
                camera.NearClipPlane = 0.05f;
                camera.FarClipPlane = 2500.0f;
                camera.ClearColor = ClearColor;
                camera.Layers = UInt64.MaxValue & ~SceneAxesHandle.LAYER; // Don't draw scene axes in this camera

                cameraController = sceneCameraSO.AddComponent<SceneCamera>();

                renderTextureGUI = new GUIRenderTexture(renderTexture);
                rtPanel.AddElement(renderTextureGUI);

		        sceneGrid = new SceneGrid(camera);
		        sceneSelection = new SceneSelection(camera);
		        sceneGizmos = new SceneGizmos(camera);
		        sceneHandles = new SceneHandles(this, camera);
		    }
		    else
		    {
		        camera.Target = renderTexture;
		        renderTextureGUI.RenderTexture = renderTexture;
		    }

            Rect2I rtBounds = new Rect2I(0, 0, width, height);
            renderTextureGUI.Bounds = rtBounds;

            sceneAxesGUI.SetPosition(width - HandleAxesGUISize - HandleAxesGUIPadding, HandleAxesGUIPadding);

            // TODO - Consider only doing the resize once user stops resizing the widget in order to reduce constant
            // render target destroy/create cycle for every single pixel.

            camera.AspectRatio = width / (float)height;

            if (profilerCamera != null)
                profilerCamera.Target = renderTexture;
	    }

        /// <summary>
        /// Parses an array of scene objects and removes elements that are children of elements that are also in the array.
        /// </summary>
        /// <param name="objects">Array containing duplicate objects as input, and array without duplicate objects as
        ///                       output.</param>
        private void CleanDuplicates(ref SceneObject[] objects)
	    {
		    List<SceneObject> cleanList = new List<SceneObject>();
		    for (int i = 0; i < objects.Length; i++)
		    {
			    bool foundParent = false;
                for (int j = 0; j < objects.Length; j++)
                {
                    SceneObject elem = objects[i];

                    while (elem != null && elem != objects[j])
                        elem = elem.Parent;

                    bool isChildOf = elem == objects[j];

				    if (i != j && isChildOf)
				    {
					    foundParent = true;
					    break;
				    }
			    }

			    if (!foundParent)
				    cleanList.Add(objects[i]);
		    }

		    objects = cleanList.ToArray();
	    }
    }
}
