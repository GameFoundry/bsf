using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class InspectorWindow : EditorWindow
    {
        private class InspectorData
        {
            public GUIFoldout foldout;
            public GUIPanelContainer container;
            public Inspector inspector;
            public bool expanded = true;
        }

        private List<InspectorData> inspectorData = new List<InspectorData>();
        private GUILayout inspectorLayout;

        internal void SetObjectToInspect(SceneObject so)
        {
            Clear();

            // TODO - Create SceneObject gui elements (name + transform)

            inspectorLayout = GUI.layout.AddLayoutY();

            Component[] allComponents = so.GetComponents();
            for (int i = 0; i < allComponents.Length; i++)
            {
                InspectorData data = new InspectorData();
                GUIPanel inspectorPanel = CreatePanel(0, 0, 0, 0);

                data.foldout = new GUIFoldout(allComponents[i].GetType().Name);
                inspectorLayout.AddElement(data.foldout);
                data.container = new GUIPanelContainer(inspectorPanel);
                inspectorLayout.AddElement(data.container);

                data.inspector = GetInspector(allComponents[i].GetType());
                data.inspector.Initialize(inspectorPanel, allComponents[i]);

                data.foldout.SetExpanded(true);
                data.foldout.OnToggled += (bool expanded) => Foldout_OnToggled(data, expanded);

                inspectorData.Add(data);
            }

            inspectorLayout.AddFlexibleSpace();

            RepositionInspectors();
        }

        void Foldout_OnToggled(InspectorData inspectorData, bool expanded)
        {
            inspectorData.expanded = expanded;
            inspectorData.inspector.SetVisible(expanded);

            RepositionInspectors();
        }

        internal void Refresh()
        {
            for (int i = 0; i < inspectorData.Count; i++)
                inspectorData[i].inspector.Refresh();
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

            if (inspectorLayout != null)
            {
                inspectorLayout.Destroy();
                inspectorLayout = null;
            }
        }

        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);

            RepositionInspectors();
        }

        private void RepositionInspectors()
        {
            int curPosition = 0;
            for (int i = 0; i < inspectorData.Count; i++)
            {
                int inspectorHeight = inspectorData[i].inspector.GetOptimalHeight();

                if (!inspectorData[i].expanded)
                    inspectorHeight = 0;

                inspectorData[i].inspector.SetArea(0, curPosition, width, inspectorHeight);
                inspectorData[i].container.SetLayoutOptions(GUIOption.FixedHeight(inspectorHeight));
                curPosition += inspectorHeight;
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
