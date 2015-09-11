using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Default implementation of the inspector used when no specified inspector is provided for the type. Inspector 
    /// displays GUI for all the inspectable fields in the object.
    /// </summary>
    internal sealed class GenericInspector : Inspector
    {
        private GUIPanel RootGUI;
        private bool isInitialized;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

        /// <inheritdoc/>
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

            GUIPanel backgroundPanel = gui.AddPanel(START_BACKGROUND_DEPTH);
            GUITexture inspectorContentBg = new GUITexture(null, EditorStyles.InspectorContentBg);
            backgroundPanel.AddElement(inspectorContentBg);

            base.Initialize(parentWindow, contentPanel, instance);
        }

        /// <inheritdoc/>
        internal override void SetVisible(bool visible)
        {
            RootGUI.Visible = visible;
        }

        /// <summary>
        /// Initializes required data the first time <see cref="Refresh"/> is called.
        /// </summary>
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

        /// <inheritdoc/>
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
    }
}
