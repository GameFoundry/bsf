using System;
using System.Collections.Generic;
using System.Data.Common;
using System.IO;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a <see cref="SceneObject"/> or for a <see cref="Resource"/>. Scene object's transform values
    /// are displayed, along with all their components and their fields.
    /// </summary>
    internal sealed class InspectorWindow : EditorWindow
    {
        /// <summary>
        /// Type of objects displayed in the window.
        /// </summary>
        private enum InspectorType
        {
            SceneObject,
            Resource,
            Multiple,
            None
        }

        /// <summary>
        /// Inspector GUI elements for a single <see cref="Component"/> in a <see cref="SceneObject"/>.
        /// </summary>
        private class InspectorComponent
        {
            public GUIToggle foldout;
            public GUIButton removeBtn;
            public GUIPanel panel;
            public Inspector inspector;
            public bool expanded = true;
            public UInt64 instanceId;
        }

        /// <summary>
        /// Inspector GUI elements for a <see cref="Resource"/>
        /// </summary>
        private class InspectorResource
        {
            public GUIPanel panel;
            public Inspector inspector;
        }

        private static readonly Color HIGHLIGHT_COLOR = new Color(1.0f, 1.0f, 1.0f, 0.5f);

        private List<InspectorComponent> inspectorComponents = new List<InspectorComponent>();
        private InspectorResource inspectorResource;
        private GUIScrollArea inspectorScrollArea;
        private GUILayout inspectorLayout;
        private GUILayoutY sceneObjectLayout;
        private GUIPanel highlightPanel;
        private GUITexture scrollAreaHighlight;

        private SceneObject activeSO;
        private GUITextBox soNameInput;
        private GUILayout soPrefabLayout;
        private bool soHasPrefab;
        private GUIFloatField soPosX;
        private GUIFloatField soPosY;
        private GUIFloatField soPosZ;
        private GUIFloatField soRotX;
        private GUIFloatField soRotY;
        private GUIFloatField soRotZ;
        private GUIFloatField soScaleX;
        private GUIFloatField soScaleY;
        private GUIFloatField soScaleZ;

        private Rect2I dropBounds;

        private InspectorType currentType = InspectorType.None;
        private Resource activeResource;

        /// <summary>
        /// Opens the inspector window from the menu bar.
        /// </summary>
        [MenuItem("Windows/Inspector", ButtonModifier.CtrlAlt, ButtonCode.I, 6000)]
        private static void OpenInspectorWindow()
        {
            OpenWindow<InspectorWindow>();
        }

        /// <summary>
        /// Name of the inspector window to display on the window title.
        /// </summary>
        /// <returns>Name of the inspector window to display on the window title.</returns>
        protected override LocString GetDisplayName()
        {
            return new LocEdString("Inspector");
        }

        /// <summary>
        /// Sets a resource whose GUI is to be displayed in the inspector. Clears any previous contents of the window.
        /// </summary>
        /// <param name="resourcePath">Resource path relative to the project of the resource to inspect.</param>
        private void SetObjectToInspect(String resourcePath)
        {
            activeResource = ProjectLibrary.Load<Resource>(resourcePath);

            if (activeResource == null)
                return;

            currentType = InspectorType.Resource;

            inspectorScrollArea = new GUIScrollArea();
            GUI.AddElement(inspectorScrollArea);
            inspectorLayout = inspectorScrollArea.Layout;

            inspectorResource = new InspectorResource();
            inspectorResource.panel = inspectorLayout.AddPanel();

            inspectorResource.inspector = InspectorUtility.GetInspector(activeResource.GetType());
            inspectorResource.inspector.Initialize(this, inspectorResource.panel, activeResource);
            inspectorResource.inspector.Refresh();

            inspectorLayout.AddFlexibleSpace();
        }

        /// <summary>
        /// Sets a scene object whose GUI is to be displayed in the inspector. Clears any previous contents of the window.
        /// </summary>
        /// <param name="so">Scene object to inspect.</param>
        private void SetObjectToInspect(SceneObject so)
        {
            if (so == null)
                return;

            currentType = InspectorType.SceneObject;
            activeSO = so;

            inspectorScrollArea = new GUIScrollArea();
            scrollAreaHighlight = new GUITexture(Builtin.WhiteTexture);
            scrollAreaHighlight.SetTint(HIGHLIGHT_COLOR);

            GUI.AddElement(inspectorScrollArea);
            highlightPanel = GUI.AddPanel(-1);
            highlightPanel.AddElement(scrollAreaHighlight);
            inspectorLayout = inspectorScrollArea.Layout;

            // SceneObject fields
            CreateSceneObjectFields();
            RefreshSceneObjectFields(true);

            // Components
            Component[] allComponents = so.GetComponents();
            for (int i = 0; i < allComponents.Length; i++)
            {
                InspectorComponent data = new InspectorComponent();
                data.instanceId = allComponents[i].InstanceId;

                data.foldout = new GUIToggle(allComponents[i].GetType().Name, EditorStyles.Foldout);
                data.removeBtn = new GUIButton(new GUIContent(EditorBuiltin.XBtnIcon), GUIOption.FixedWidth(30));

                GUILayoutX titleLayout = inspectorLayout.AddLayoutX();
                titleLayout.AddElement(data.foldout);
                titleLayout.AddElement(data.removeBtn);

                data.panel = inspectorLayout.AddPanel();
                data.inspector = InspectorUtility.GetInspector(allComponents[i].GetType());
                data.inspector.Initialize(this, data.panel, allComponents[i]);
                data.foldout.Value = true;

                Type curComponentType = allComponents[i].GetType();
                data.foldout.OnToggled += (bool expanded) => OnComponentFoldoutToggled(data, expanded);
                data.removeBtn.OnClick += () => OnComponentRemoveClicked(curComponentType);

                inspectorComponents.Add(data);

                inspectorComponents[i].inspector.Refresh();
            }

            inspectorLayout.AddFlexibleSpace();

            dropBounds = inspectorScrollArea.Bounds;
            scrollAreaHighlight.Bounds = dropBounds;
        }

        /// <summary>
        /// Creates GUI elements required for displaying <see cref="SceneObject"/> fields like name, prefab data and 
        /// transform (position, rotation, scale). Assumes that necessary inspector scroll area layout has already been 
        /// created.
        /// </summary>
        private void CreateSceneObjectFields()
        {
            GUIPanel sceneObjectPanel = inspectorLayout.AddPanel();
            sceneObjectPanel.SetHeight(GetTitleBounds().height);

            sceneObjectLayout = sceneObjectPanel.AddLayoutY();
            sceneObjectLayout.SetPosition(5, 5);

            GUIPanel sceneObjectBgPanel = sceneObjectPanel.AddPanel(1);

            GUILayoutX nameLayout = sceneObjectLayout.AddLayoutX();
            GUILabel nameLbl = new GUILabel(new LocEdString("Name"), GUIOption.FixedWidth(50));
            soNameInput = new GUITextBox(false, GUIOption.FlexibleWidth(180));
            soNameInput.Text = activeSO.Name;
            soNameInput.OnChanged += (x) => { if (activeSO != null) activeSO.Name = x; };

            nameLayout.AddElement(nameLbl);
            nameLayout.AddElement(soNameInput);
            nameLayout.AddFlexibleSpace();

            soPrefabLayout = sceneObjectLayout.AddLayoutX();

            GUILayoutX positionLayout = sceneObjectLayout.AddLayoutX();
            GUILabel positionLbl = new GUILabel(new LocEdString("Position"), GUIOption.FixedWidth(50));
            soPosX = new GUIFloatField(new LocEdString("X"), 10, "", GUIOption.FixedWidth(60));
            soPosY = new GUIFloatField(new LocEdString("Y"), 10, "", GUIOption.FixedWidth(60));
            soPosZ = new GUIFloatField(new LocEdString("Z"), 10, "", GUIOption.FixedWidth(60));

            soPosX.OnChanged += (x) => OnPositionChanged(0, x);
            soPosY.OnChanged += (y) => OnPositionChanged(1, y);
            soPosZ.OnChanged += (z) => OnPositionChanged(2, z);

            positionLayout.AddElement(positionLbl);
            positionLayout.AddElement(soPosX);
            positionLayout.AddSpace(10);
            positionLayout.AddFlexibleSpace();
            positionLayout.AddElement(soPosY);
            positionLayout.AddSpace(10);
            positionLayout.AddFlexibleSpace();
            positionLayout.AddElement(soPosZ);
            positionLayout.AddFlexibleSpace();

            GUILayoutX rotationLayout = sceneObjectLayout.AddLayoutX();
            GUILabel rotationLbl = new GUILabel(new LocEdString("Rotation"), GUIOption.FixedWidth(50));
            soRotX = new GUIFloatField(new LocEdString("X"), 10, "", GUIOption.FixedWidth(60));
            soRotY = new GUIFloatField(new LocEdString("Y"), 10, "", GUIOption.FixedWidth(60));
            soRotZ = new GUIFloatField(new LocEdString("Z"), 10, "", GUIOption.FixedWidth(60));

            soRotX.OnChanged += (x) => OnRotationChanged(0, x);
            soRotY.OnChanged += (y) => OnRotationChanged(1, y);
            soRotZ.OnChanged += (z) => OnRotationChanged(2, z);

            rotationLayout.AddElement(rotationLbl);
            rotationLayout.AddElement(soRotX);
            rotationLayout.AddSpace(10);
            rotationLayout.AddFlexibleSpace();
            rotationLayout.AddElement(soRotY);
            rotationLayout.AddSpace(10);
            rotationLayout.AddFlexibleSpace();
            rotationLayout.AddElement(soRotZ);
            rotationLayout.AddFlexibleSpace();

            GUILayoutX scaleLayout = sceneObjectLayout.AddLayoutX();
            GUILabel scaleLbl = new GUILabel(new LocEdString("Scale"), GUIOption.FixedWidth(50));
            soScaleX = new GUIFloatField(new LocEdString("X"), 10, "", GUIOption.FixedWidth(60));
            soScaleY = new GUIFloatField(new LocEdString("Y"), 10, "", GUIOption.FixedWidth(60));
            soScaleZ = new GUIFloatField(new LocEdString("Z"), 10, "", GUIOption.FixedWidth(60));

            soScaleX.OnChanged += (x) => OnScaleChanged(0, x);
            soScaleY.OnChanged += (y) => OnScaleChanged(1, y);
            soScaleX.OnChanged += (z) => OnScaleChanged(2, z);

            scaleLayout.AddElement(scaleLbl);
            scaleLayout.AddElement(soScaleX);
            scaleLayout.AddSpace(10);
            scaleLayout.AddFlexibleSpace();
            scaleLayout.AddElement(soScaleY);
            scaleLayout.AddSpace(10);
            scaleLayout.AddFlexibleSpace();
            scaleLayout.AddElement(soScaleZ);
            scaleLayout.AddFlexibleSpace();

            sceneObjectLayout.AddFlexibleSpace();

            inspectorLayout.AddSpace(5);

            GUITexture titleBg = new GUITexture(null, EditorStyles.InspectorTitleBg);
            sceneObjectBgPanel.AddElement(titleBg);
        }

        /// <summary>
        /// Updates contents of the scene object specific fields (name, position, rotation, etc.)
        /// </summary>
        /// <param name="forceUpdate">If true, the GUI elements will be updated regardless of whether a change was
        ///                           detected or not.</param>
        private void RefreshSceneObjectFields(bool forceUpdate)
        {
            if (activeSO == null)
                return;

            soNameInput.Text = activeSO.Name;

            bool hasPrefab = PrefabUtility.HasPrefabLink(activeSO);
            if (soHasPrefab != hasPrefab || forceUpdate)
            {
                int numChildren = soPrefabLayout.ChildCount;
                for (int i = 0; i < numChildren; i++)
                    soPrefabLayout.GetChild(0).Destroy();

                GUILabel prefabLabel =new GUILabel(new LocEdString("Prefab"), GUIOption.FixedWidth(50));
                soPrefabLayout.AddElement(prefabLabel);

                if (hasPrefab)
                {
                    GUIButton btnApplyPrefab = new GUIButton(new LocEdString("Apply"), GUIOption.FixedWidth(60));
                    GUIButton btnRevertPrefab = new GUIButton(new LocEdString("Revert"), GUIOption.FixedWidth(60));
                    GUIButton btnBreakPrefab = new GUIButton(new LocEdString("Break"), GUIOption.FixedWidth(60));

                    btnApplyPrefab.OnClick += () => PrefabUtility.ApplyPrefab(activeSO);
                    btnRevertPrefab.OnClick += () => PrefabUtility.RevertPrefab(activeSO);
                    btnBreakPrefab.OnClick += () => PrefabUtility.BreakPrefab(activeSO);

                    soPrefabLayout.AddElement(btnApplyPrefab);
                    soPrefabLayout.AddElement(btnRevertPrefab);
                    soPrefabLayout.AddElement(btnBreakPrefab);
                }
                else
                {
                    GUILabel noPrefabLabel = new GUILabel("None");
                    soPrefabLayout.AddElement(noPrefabLabel);
                }

                soHasPrefab = hasPrefab;
            }

            Vector3 position;
            Vector3 angles;
            if (EditorApplication.ActiveCoordinateMode == HandleCoordinateMode.World)
            {
                position = activeSO.Position;
                angles = activeSO.Rotation.ToEuler();
            }
            else
            {
                position = activeSO.LocalPosition;
                angles = activeSO.LocalRotation.ToEuler();
            }

            Vector3 scale = activeSO.LocalScale;

            soPosX.Value = position.x;
            soPosY.Value = position.y;
            soPosZ.Value = position.z;

            soRotX.Value = angles.x;
            soRotY.Value = angles.y;
            soRotZ.Value = angles.z;

            soScaleX.Value = scale.x;
            soScaleY.Value = scale.y;
            soScaleZ.Value = scale.z;
        }

        private void OnInitialize()
        {
            Selection.OnSelectionChanged += OnSelectionChanged;

            OnSelectionChanged(new SceneObject[0], new string[0]);
        }

        private void OnDestroy()
        {
            Selection.OnSelectionChanged -= OnSelectionChanged;
        }

        private void OnEditorUpdate()
        {
            if (currentType == InspectorType.SceneObject)
            {
                Component[] allComponents = activeSO.GetComponents();
                bool requiresRebuild = allComponents.Length != inspectorComponents.Count;

                if (!requiresRebuild)
                {
                    for (int i = 0; i < inspectorComponents.Count; i++)
                    {
                        if (inspectorComponents[i].instanceId != allComponents[i].InstanceId)
                        {
                            requiresRebuild = true;
                            break;
                        }
                    }
                }

                if (requiresRebuild)
                {
                    SceneObject so = activeSO;
                    Clear();
                    SetObjectToInspect(so);
                }
                else
                {
                    RefreshSceneObjectFields(false);

                    for (int i = 0; i < inspectorComponents.Count; i++)
                    {
                        inspectorComponents[i].inspector.Refresh();
                    }
                }
            }
            else if (currentType == InspectorType.Resource)
            {
                inspectorResource.inspector.Refresh();
            }

            // Detect drag and drop
            bool isDraggingOver = false;

            if (activeSO != null && inspectorScrollArea != null)
            {
                Vector2I windowPos = ScreenToWindowPos(Input.PointerPosition);
                ScriptCode droppedCodeFile = null;
                string droppedCodeFileName = "";

                if (DragDrop.DragInProgress && DragDrop.Type == DragDropType.Resource)
                {
                    isDraggingOver = dropBounds.Contains(windowPos);
                }
                else if (DragDrop.DropInProgress && DragDrop.Type == DragDropType.Resource)
                {
                    ResourceDragDropData dragData = DragDrop.Data as ResourceDragDropData;
                    if (dragData != null)
                    {
                        foreach (var resPath in dragData.Paths)
                        {
                            LibraryEntry entry = ProjectLibrary.GetEntry(resPath);
                            FileEntry fileEntry = entry as FileEntry;
                            if (fileEntry != null)
                            {
                                if (fileEntry.ResType == ResourceType.ScriptCode)
                                {
                                    droppedCodeFile = ProjectLibrary.Load<ScriptCode>(resPath);
                                    droppedCodeFileName = fileEntry.Name;
                                    break;
                                }
                            }
                        }
                    }
                }

                if (droppedCodeFile != null)
                {
                    Type droppedComponentType = null;
                    Type[] droppedTypes = droppedCodeFile.Types;
                    foreach (var type in droppedTypes)
                    {
                        if (type.IsSubclassOf(typeof(Component)))
                        {
                            droppedComponentType = type;
                            break;
                        }
                    }

                    if (droppedComponentType != null)
                    {
                        UndoRedo.RecordSO(activeSO, "Added component " + droppedComponentType.Name);
                        activeSO.AddComponent(droppedComponentType);
                    }
                    else
                        Debug.LogWarning("Cannot find a Component in " + droppedCodeFileName);
                }
            }

            if (scrollAreaHighlight != null)
                scrollAreaHighlight.Visible = isDraggingOver;
        }

        /// <summary>
        /// Triggered when the user selects a new resource or a scene object, or deselects everything.
        /// </summary>
        /// <param name="objects">A set of new scene objects that were selected.</param>
        /// <param name="paths">A set of absolute resource paths that were selected.</param>
        private void OnSelectionChanged(SceneObject[] objects, string[] paths)
        {
            Clear();

            if (objects.Length == 0 && paths.Length == 0)
            {
                currentType = InspectorType.None;
                inspectorScrollArea = new GUIScrollArea();
                GUI.AddElement(inspectorScrollArea);
                inspectorLayout = inspectorScrollArea.Layout;

                inspectorLayout.AddFlexibleSpace();
                GUILayoutX layoutMsg = inspectorLayout.AddLayoutX();
                layoutMsg.AddFlexibleSpace();
                layoutMsg.AddElement(new GUILabel(new LocEdString("No object selected")));
                layoutMsg.AddFlexibleSpace();
                inspectorLayout.AddFlexibleSpace();
            }
            else if ((objects.Length + paths.Length) > 1)
            {
                currentType = InspectorType.None;
                inspectorScrollArea = new GUIScrollArea();
                GUI.AddElement(inspectorScrollArea);
                inspectorLayout = inspectorScrollArea.Layout;

                inspectorLayout.AddFlexibleSpace();
                GUILayoutX layoutMsg = inspectorLayout.AddLayoutX();
                layoutMsg.AddFlexibleSpace();
                layoutMsg.AddElement(new GUILabel(new LocEdString("Multiple objects selected")));
                layoutMsg.AddFlexibleSpace();
                inspectorLayout.AddFlexibleSpace();
            }
            else if (objects.Length == 1)
            {
                SetObjectToInspect(objects[0]);
            }
            else if (paths.Length == 1)
            {
                SetObjectToInspect(paths[0]);
            }
        }

        /// <summary>
        /// Triggered when the user closes or expands a component foldout, making the component fields visible or hidden.
        /// </summary>
        /// <param name="inspectorData">Contains GUI data for the component that was toggled.</param>
        /// <param name="expanded">Determines whether to display or hide component contents.</param>
        private void OnComponentFoldoutToggled(InspectorComponent inspectorData, bool expanded)
        {
            inspectorData.expanded = expanded;
            inspectorData.inspector.SetVisible(expanded);
        }

        /// <summary>
        /// Triggered when the user clicks the component remove button. Removes that component from the active scene object.
        /// </summary>
        /// <param name="componentType">Type of the component to remove.</param>
        private void OnComponentRemoveClicked(Type componentType)
        {
            if (activeSO != null)
            {
                UndoRedo.RecordSO(activeSO, "Removed component " + componentType.Name);
                activeSO.RemoveComponent(componentType);
            }
        }

        /// <summary>
        /// Destroys all inspector GUI elements.
        /// </summary>
        internal void Clear()
        {
            for (int i = 0; i < inspectorComponents.Count; i++)
            {
                inspectorComponents[i].foldout.Destroy();
                inspectorComponents[i].removeBtn.Destroy();
                inspectorComponents[i].inspector.Destroy();
            }

            inspectorComponents.Clear();

            if (inspectorResource != null)
            {
                inspectorResource.inspector.Destroy();
                inspectorResource = null;
            }

            if (inspectorScrollArea != null)
            {
                inspectorScrollArea.Destroy();
                inspectorScrollArea = null;
            }

            if (scrollAreaHighlight != null)
            {
                scrollAreaHighlight.Destroy();
                scrollAreaHighlight = null;
            }

            if (highlightPanel != null)
            {
                highlightPanel.Destroy();
                highlightPanel = null;
            }

            activeSO = null;
            soNameInput = null;
            soPrefabLayout = null;
            soHasPrefab = false;
            soPosX = null;
            soPosY = null;
            soPosZ = null;
            soRotX = null;
            soRotY = null;
            soRotZ = null;
            soScaleX = null;
            soScaleY = null;
            soScaleZ = null;
            dropBounds = new Rect2I();
            sceneObjectLayout = null;

            activeResource = null;
            currentType = InspectorType.None;
        }

        /// <summary>
        /// Returns the size of the title bar area that is displayed for <see cref="SceneObject"/> specific fields.
        /// </summary>
        /// <returns>Area of the title bar, relative to the window.</returns>
        private Rect2I GetTitleBounds()
        {
            return new Rect2I(0, 0, Width, 115);
        }

        /// <summary>
        /// Triggered when the position value in the currently active <see cref="SceneObject"/> changes. Updates the 
        /// necessary GUI elements.
        /// </summary>
        /// <param name="idx">Index of the coordinate that was changed.</param>
        /// <param name="value">New value of the field.</param>
        private void OnPositionChanged(int idx, float value)
        {
            if (activeSO == null)
                return;

            if (EditorApplication.ActiveCoordinateMode == HandleCoordinateMode.World)
            {
                Vector3 position = activeSO.Position;
                position[idx] = value;
                activeSO.Position = position;
            }
            else
            {
                Vector3 position = activeSO.LocalPosition;
                position[idx] = value;
                activeSO.LocalPosition = position;
            }
        }

        /// <summary>
        /// Triggered when the rotation value in the currently active <see cref="SceneObject"/> changes. Updates the 
        /// necessary GUI elements.
        /// </summary>
        /// <param name="idx">Index of the euler angle that was changed (0 - X, 1 - Y, 2 - Z).</param>
        /// <param name="value">New value of the field.</param>
        private void OnRotationChanged(int idx, float value)
        {
            if (activeSO == null)
                return;

            if (EditorApplication.ActiveCoordinateMode == HandleCoordinateMode.World)
            {
                Vector3 angles = activeSO.Rotation.ToEuler();
                angles[idx] = value;
                activeSO.Rotation = Quaternion.FromEuler(angles);
            }
            else
            {
                Vector3 angles = activeSO.LocalRotation.ToEuler();
                angles[idx] = value;
                activeSO.LocalRotation = Quaternion.FromEuler(angles);
            }
        }

        /// <summary>
        /// Triggered when the scale value in the currently active <see cref="SceneObject"/> changes. Updates the 
        /// necessary GUI elements.
        /// </summary>
        /// <param name="idx">Index of the coordinate that was changed.</param>
        /// <param name="value">New value of the field.</param>
        private void OnScaleChanged(int idx, float value)
        {
            if (activeSO == null)
                return;

            Vector3 scale = activeSO.LocalScale;
            scale[idx] = value;
            activeSO.LocalScale = scale;
        }

        /// <inheritdoc/>
        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);

            if(inspectorScrollArea != null)
                dropBounds = inspectorScrollArea.Bounds;

            if (scrollAreaHighlight != null)
                scrollAreaHighlight.Bounds = dropBounds;
        }
    }
}
