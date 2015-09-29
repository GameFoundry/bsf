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
        private bool isInitialized;
        private bool isEmpty = true;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

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
                    isEmpty = false;
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

        /// <inheritdoc/>
        internal override void SetVisible(bool visible)
        {
            RootGUI.Enabled = !isEmpty && visible;
        }
    }
}
