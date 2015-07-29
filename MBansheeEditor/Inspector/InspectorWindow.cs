using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class InspectorWindow : EditorWindow
    {
        private enum InspectorType
        {
            SceneObject,
            Resource,
            Multiple,
            None
        }

        private class InspectorComponent
        {
            public GUIComponentFoldout foldout;
            public GUIPanel panel;
            public Inspector inspector;
            public bool expanded = true;
            public UInt64 instanceId;
        }

        private class InspectorResource
        {
            public GUIPanel panel;
            public Inspector inspector;
        }

        private List<InspectorComponent> inspectorComponents = new List<InspectorComponent>();
        private InspectorResource inspectorResource;
        private GUIScrollArea inspectorScrollArea;
        private GUILayout inspectorLayout;

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

        private InspectorType currentType = InspectorType.None;
        private Resource activeResource;

        [MenuItem("Windows/Inspector", ButtonModifier.CtrlAlt, ButtonCode.I)]
        private static void OpenInspectorWindow()
        {
            OpenWindow<InspectorWindow>();
        }

        protected override LocString GetDisplayName()
        {
            return "Inspector";
        }

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

            inspectorResource.inspector = GetInspector(activeResource.GetType());
            inspectorResource.inspector.Initialize(this, inspectorResource.panel, activeResource);
            inspectorResource.inspector.Refresh();

            inspectorLayout.AddFlexibleSpace();
        }

        private void SetObjectToInspect(SceneObject so)
        {
            if (so == null)
                return;

            currentType = InspectorType.SceneObject;
            activeSO = so;

            inspectorScrollArea = new GUIScrollArea();
            GUI.AddElement(inspectorScrollArea);
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

                data.foldout = new GUIComponentFoldout(allComponents[i].GetType().Name);
                inspectorLayout.AddElement(data.foldout);
                data.panel = inspectorLayout.AddPanel();
                
                data.inspector = GetInspector(allComponents[i].GetType());
                data.inspector.Initialize(this, data.panel, allComponents[i]);

                data.foldout.SetExpanded(true);
                data.foldout.OnToggled += (bool expanded) => OnComponentFoldoutToggled(data, expanded);

                inspectorComponents.Add(data);

                inspectorComponents[i].inspector.Refresh();
            }

            inspectorLayout.AddFlexibleSpace();
        }

        private void OnComponentFoldoutToggled(InspectorComponent inspectorData, bool expanded)
        {
            inspectorData.expanded = expanded;
            inspectorData.inspector.SetVisible(expanded);
        }

        private void CreateSceneObjectFields()
        {
            GUIPanel sceneObjectPanel = inspectorLayout.AddPanel();
            GUILayoutY sceneObjectLayout = sceneObjectPanel.AddLayoutY();

            GUILayoutX nameLayout = sceneObjectLayout.AddLayoutX();
            GUILabel nameLbl = new GUILabel("Name", GUIOption.FixedWidth(70));
            soNameInput = new GUITextBox(false, GUIOption.FlexibleWidth(200));
            soNameInput.Text = activeSO.Name;
            soNameInput.OnChanged += (x) => { if (activeSO != null) activeSO.Name = x; };

            nameLayout.AddElement(nameLbl);
            nameLayout.AddElement(soNameInput);
            nameLayout.AddFlexibleSpace();

            soPrefabLayout = sceneObjectLayout.AddLayoutX();

            GUILayoutX positionLayout = sceneObjectLayout.AddLayoutX();
            GUILabel positionLbl = new GUILabel("Position", GUIOption.FixedWidth(70));
            soPosX = new GUIFloatField(new GUIContent("X"), 10, "", GUIOption.FlexibleWidth(50));
            soPosY = new GUIFloatField(new GUIContent("Y"), 10, "", GUIOption.FlexibleWidth(50));
            soPosZ = new GUIFloatField(new GUIContent("Z"), 10, "", GUIOption.FlexibleWidth(50));

            soPosX.OnChanged += (x) => OnPositionChanged(0, x);
            soPosY.OnChanged += (y) => OnPositionChanged(1, y);
            soPosZ.OnChanged += (z) => OnPositionChanged(2, z);

            positionLayout.AddElement(positionLbl);
            positionLayout.AddElement(soPosX);
            positionLayout.AddSpace(10);
            positionLayout.AddElement(soPosY);
            positionLayout.AddSpace(10);
            positionLayout.AddElement(soPosZ);
            positionLayout.AddFlexibleSpace();

            GUILayoutX rotationLayout = sceneObjectLayout.AddLayoutX();
            GUILabel rotationLbl = new GUILabel("Rotation", GUIOption.FixedWidth(70));
            soRotX = new GUIFloatField(new GUIContent("X"), 10, "", GUIOption.FlexibleWidth(50));
            soRotY = new GUIFloatField(new GUIContent("Y"), 10, "", GUIOption.FlexibleWidth(50));
            soRotZ = new GUIFloatField(new GUIContent("Z"), 10, "", GUIOption.FlexibleWidth(50));

            soRotX.OnChanged += (x) => OnRotationChanged(0, x);
            soRotY.OnChanged += (y) => OnRotationChanged(1, y);
            soRotZ.OnChanged += (z) => OnRotationChanged(2, z);

            rotationLayout.AddElement(rotationLbl);
            rotationLayout.AddElement(soRotX);
            rotationLayout.AddSpace(10);
            rotationLayout.AddElement(soRotY);
            rotationLayout.AddSpace(10);
            rotationLayout.AddElement(soRotZ);
            rotationLayout.AddFlexibleSpace();

            GUILayoutX scaleLayout = sceneObjectLayout.AddLayoutX();
            GUILabel scaleLbl = new GUILabel("Scale", GUIOption.FixedWidth(70));
            soScaleX = new GUIFloatField(new GUIContent("X"), 10, "", GUIOption.FlexibleWidth(50));
            soScaleY = new GUIFloatField(new GUIContent("Y"), 10, "", GUIOption.FlexibleWidth(50));
            soScaleZ = new GUIFloatField(new GUIContent("Z"), 10, "", GUIOption.FlexibleWidth(50));

            soScaleX.OnChanged += (x) => OnScaleChanged(0, x);
            soScaleY.OnChanged += (y) => OnScaleChanged(1, y);
            soScaleX.OnChanged += (z) => OnScaleChanged(2, z);

            scaleLayout.AddElement(scaleLbl);
            scaleLayout.AddElement(soScaleX);
            scaleLayout.AddSpace(10);
            scaleLayout.AddElement(soScaleY);
            scaleLayout.AddSpace(10);
            scaleLayout.AddElement(soScaleZ);
            scaleLayout.AddFlexibleSpace();

            inspectorLayout.AddSpace(15);
        }

        private void RefreshSceneObjectFields(bool forceUpdate)
        {
            if (activeSO == null)
                return;

            soNameInput.Text = activeSO.Name;

            bool hasPrefab = PrefabUtility.HasPrefabLink(activeSO);
            if (soHasPrefab != hasPrefab || forceUpdate)
            {
                int numChildren = soPrefabLayout.GetNumChildren();
                for (int i = 0; i < numChildren; i++)
                    soPrefabLayout.GetChild(0).Destroy();

                GUILabel prefabLabel =new GUILabel("Prefab", GUIOption.FixedWidth(70));
                soPrefabLayout.AddElement(prefabLabel);

                //if (hasPrefab) // TODO - Disabled check for preview purposes
                {
                    GUIButton btnApplyPrefab = new GUIButton("Apply");
                    GUIButton btnRevertPrefab = new GUIButton("Revert");
                    GUIButton btnBreakPrefab = new GUIButton("Break");

                    btnApplyPrefab.OnClick += () => PrefabUtility.ApplyPrefab(activeSO);
                    btnRevertPrefab.OnClick += () => PrefabUtility.RevertPrefab(activeSO);
                    btnBreakPrefab.OnClick += () => PrefabUtility.BreakPrefab(activeSO);

                    soPrefabLayout.AddElement(btnApplyPrefab);
                    soPrefabLayout.AddElement(btnRevertPrefab);
                    soPrefabLayout.AddElement(btnBreakPrefab);
                }
                //else
                //{
                //    GUILabel noPrefabLabel = new GUILabel("None");
                //    soPrefabLayout.AddElement(noPrefabLabel);
                //}

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
                    SetObjectToInspect(activeSO);
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
        }

        private void OnSelectionChanged(SceneObject[] objects, string[] paths)
        {
            Clear();

            if (objects.Length == 0 && paths.Length == 0)
            {
                currentType = InspectorType.None;
                inspectorScrollArea = new GUIScrollArea();
                GUI.AddElement(inspectorScrollArea);
                inspectorLayout = inspectorScrollArea.Layout;
                inspectorLayout.AddElement(new GUILabel("No object selected"));
                inspectorLayout.AddFlexibleSpace();
            }
            else if ((objects.Length + paths.Length) > 1)
            {
                currentType = InspectorType.None;
                inspectorScrollArea = new GUIScrollArea();
                GUI.AddElement(inspectorScrollArea);
                inspectorLayout = inspectorScrollArea.Layout;
                inspectorLayout.AddElement(new GUILabel("Multiple objects selected"));
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

        internal void Destroy()
        {
            Clear();
        }

        internal void Clear()
        {
            for (int i = 0; i < inspectorComponents.Count; i++)
            {
                inspectorComponents[i].foldout.Destroy();
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

            activeResource = null;
            currentType = InspectorType.None;
        }

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

        private void OnScaleChanged(int idx, float value)
        {
            if (activeSO == null)
                return;

            Vector3 scale = activeSO.LocalScale;
            scale[idx] = value;
            activeSO.LocalScale = scale;
        }

        private Inspector GetInspector(Type type)
        {
            // TODO - Check if type has a custom inspector
            // and return the custom inspector, otherwise create a generic one

            return new GenericInspector();
        }
    }
}
