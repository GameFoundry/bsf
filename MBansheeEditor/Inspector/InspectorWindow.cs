using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class InspectorWindow : EditorWindow
    {
        private class InspectorData
        {
            public GUIComponentFoldout foldout;
            public GUIPanel panel;
            public Inspector inspector;
            public bool expanded = true;
        }

        private List<InspectorData> inspectorData = new List<InspectorData>();
        private GUIScrollArea inspectorScrollArea;
        private GUILayout inspectorLayout;

        [MenuItem("Windows/Inspector", ButtonModifier.CtrlAlt, ButtonCode.I)]
        private static void OpenHierarchyWindow()
        {
            OpenWindow<HierarchyWindow>();
        }

        internal void SetObjectToInspect(SceneObject so)
        {
            Clear();

            // TODO - Create SceneObject gui elements (name + transform)

            inspectorScrollArea = new GUIScrollArea();
            GUI.AddElement(inspectorScrollArea);
            inspectorLayout = inspectorScrollArea.Layout;

            Component[] allComponents = so.GetComponents();
            for (int i = 0; i < allComponents.Length; i++)
            {
                InspectorData data = new InspectorData();

                data.foldout = new GUIComponentFoldout(allComponents[i].GetType().Name);
                inspectorLayout.AddElement(data.foldout);
                data.panel = inspectorLayout.AddPanel();
                
                data.inspector = GetInspector(allComponents[i].GetType());
                data.inspector.Initialize(this, data.panel, allComponents[i]);

                data.foldout.SetExpanded(true);
                data.foldout.OnToggled += (bool expanded) => OnComponentFoldoutToggled(data, expanded);

                inspectorData.Add(data);

                inspectorData[i].inspector.Refresh();
            }

            inspectorLayout.AddFlexibleSpace();
        }

        private void OnComponentFoldoutToggled(InspectorData inspectorData, bool expanded)
        {
            inspectorData.expanded = expanded;
            inspectorData.inspector.SetVisible(expanded);
        }

        private void OnEditorUpdate()
        {
            for (int i = 0; i < inspectorData.Count; i++)
            {
                inspectorData[i].inspector.Refresh();
            }
        }

        internal void Destroy()
        {
            // TODO - Destroy SceneObject GUI elements

            Clear();
        }

        internal void Clear()
        {
            for (int i = 0; i < inspectorData.Count; i++)
            {
                inspectorData[i].foldout.Destroy();
                inspectorData[i].inspector.Destroy();
            }

            inspectorData.Clear();

            if (inspectorScrollArea != null)
            {
                inspectorScrollArea.Destroy();
                inspectorScrollArea = null;
            }
        }

        private Inspector GetInspector(Type type)
        {
            // TODO - Check if type has a custom inspector
            // and return the custom inspector, otherwise create a generic one

            return new GenericInspector();
        }
    }
}
