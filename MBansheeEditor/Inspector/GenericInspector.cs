using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class GenericInspector : Inspector
    {
        private GUIPanel RootGUI;
        private bool isInitialized;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

        internal override void Initialize(InspectorWindow parentWindow, GUIPanel gui, object instance)
        {
            RootGUI = gui;

            GUILayout contentLayoutX = gui.AddLayoutX();
            contentLayoutX.AddSpace(5);
            GUILayout contentLayoutY = contentLayoutX.AddLayoutY();
            contentLayoutY.AddSpace(5);
            GUIPanel contentPanel = contentLayoutY.AddPanel();
            contentLayoutY.AddSpace(5);
            contentLayoutX.AddSpace(5);

            GUIPanel backgroundPanel = gui.AddPanel(1);
            GUITexture inspectorContentBg = new GUITexture(null, EditorStyles.InspectorContentBg);
            backgroundPanel.AddElement(inspectorContentBg);

            base.Initialize(parentWindow, contentPanel, instance);
        }

        internal override void SetVisible(bool visible)
        {
            RootGUI.Visible = visible;
        }

        private void Initialize()
        {
            if (referencedObject != null)
            {
                SerializableObject serializableObject = new SerializableObject(referencedObject.GetType(), referencedObject);

                foreach (var field in serializableObject.Fields)
                {
                    if (!field.Inspectable)
                        continue;

                    inspectableFields.Add(InspectableField.CreateInspectable(field.Name, 0, new InspectableFieldLayout(layout), field.GetProperty()));
                }
            }

            isInitialized = true;
        }

        internal override bool Refresh()
        {
            if (!isInitialized)
                Initialize();

            bool anythingModified = false;

            int currentIndex = 0;
            foreach (var field in inspectableFields)
            {
                anythingModified |= field.Refresh(currentIndex);
                currentIndex += field.GetNumLayoutElements();
            }

            return anythingModified;
        }

        internal override int GetOptimalHeight()
        {
            return GUILayoutUtility.CalculateOptimalSize(layout).y;
        }
    }
}
