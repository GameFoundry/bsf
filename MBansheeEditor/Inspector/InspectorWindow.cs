using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class InspectorWindow : EditorWindow
    {
        private List<Inspector> inspectors = new List<Inspector>();

        internal void Initialize(SceneObject so)
        {
            Clear();

            // TODO - Create SceneObject gui elements (name + transform)

            Component[] allComponents = so.GetComponents();
            for (int i = 0; i < allComponents.Length; i++)
            {
                // TODO
                //  - Create component foldout
                //  - Hook up the foldout so when clicked it will expand/collapse the custom inspector or child object

                Inspector inspector = GetInspector(allComponents[i].GetType());
                inspector.Initialize(CreatePanel(0, 0, 0, 0), allComponents[i]);

                inspectors.Add(inspector);
            }

            RepositionInspectors();
        }

        internal void Refresh()
        {
            for (int i = 0; i < inspectors.Count; i++)
                inspectors[i].Refresh();
        }

        internal void Destroy()
        {
            // TODO - Destroy SceneObject GUI elements

            Clear();
        }

        internal void Clear()
        {
            for (int i = 0; i < inspectors.Count; i++)
                inspectors[i].Destroy();

            inspectors.Clear();
        }

        protected override void WindowResized(int width, int height)
        {
            RepositionInspectors();
        }

        private void RepositionInspectors()
        {
            int curPosition = 0;
            for (int i = 0; i < inspectors.Count; i++)
            {
                int inspectorHeight = inspectors[i].GetOptimalHeight();

                inspectors[i].SetArea(0, curPosition, width, inspectorHeight);
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
