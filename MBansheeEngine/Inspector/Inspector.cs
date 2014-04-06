using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public class Inspector
    {
        private GUIElement[] guiElements;
        private InspectableObject[] childObjects;
        private CustomInspector[] customInspectors;

        public Inspector(SceneObject so, GUILayout layout)
        {
            // TODO - Create SceneObject gui elements (name + transform)
            
            List<CustomInspector> customInspectors = new List<CustomInspector>();
            List<InspectableObject> childObjects = new List<InspectableObject>();

            Component[] allComponents = so.GetComponents();
            for (int i = 0; i < allComponents.Length; i++)
            {
                // TODO
                //  - Create component foldout
                //  - Hook up the foldout so when clicked it will expand/collapse the custom inspector or child object

                if (HasCustomInspector(allComponents[i].GetType()))
                {
                    customInspectors.Add(CreateCustomInspector(allComponents[i]));
                }
                else
                {
                    childObjects.Add(new InspectableObject(allComponents[i]));
                }
            }

            this.customInspectors = customInspectors.ToArray();
            this.childObjects = childObjects.ToArray();
        }

        public void Refresh()
        {
            for (int i = 0; i < childObjects.Length; i++)
                childObjects[i].Refresh();

            for (int i = 0; i < customInspectors.Length; i++)
                customInspectors[i].Refresh();
        }

        public void Destroy()
        {
            // TODO - Destroy SceneObject GUI elements

            for (int i = 0; i < childObjects.Length; i++)
                childObjects[i].Destroy();

            for (int i = 0; i < customInspectors.Length; i++)
                customInspectors[i].Destroy();
        }

        private bool HasCustomInspector(Type type)
        {
            // TODO - Check if Component (or some other type) has a custom inspector
            return false;
        }

        private CustomInspector CreateCustomInspector(Component component)
        {
            // Find and create a custom inspector
            return null;
        }

        // TODO - Ensure all GUI elements are properly cleaned up when this is destroyed
    }
}
