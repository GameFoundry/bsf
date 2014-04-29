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
            public Inspector inspector;
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

                data.foldout = new GUIFoldout(allComponents[i].GetType().Name);
                inspectorLayout.AddElement(data.foldout);

                data.inspector = GetInspector(allComponents[i].GetType());
                data.inspector.Initialize(CreatePanel(0, 0, 0, 0), allComponents[i]);

                data.foldout.OnToggled += (bool expanded) => Foldout_OnToggled(data.inspector, expanded);

                inspectorData.Add(data);
            }

            RepositionInspectors();
        }

        void Foldout_OnToggled(Inspector inspector, bool expanded)
        {
            inspector.SetVisible(expanded);
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
            RepositionInspectors();
        }

        private void RepositionInspectors()
        {
            int curPosition = 0;
            for (int i = 0; i < inspectorData.Count; i++)
            {
                int inspectorHeight = inspectorData[i].inspector.GetOptimalHeight();

                inspectorData[i].inspector.SetArea(0, curPosition, width, inspectorHeight);
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
