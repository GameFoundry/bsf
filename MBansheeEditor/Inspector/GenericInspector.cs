using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Default implementation of the inspector used when no specified inspector is provided for the type. Inspector 
    /// displays GUI for all the inspectable fields in the object.
    /// </summary>
    internal sealed class GenericInspector : Inspector
    {
        private bool isEmpty = true;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            if (referencedObject != null)
            {
                int currentIndex = 0;
                SerializableObject serializableObject = new SerializableObject(referencedObject.GetType(), referencedObject);
                foreach (var field in serializableObject.Fields)
                {
                    if (!field.Inspectable)
                        continue;

                    InspectableField inspectableField = InspectableField.CreateInspectable(field.Name, currentIndex, 0,
                        new InspectableFieldLayout(layout), field.GetProperty());

                    inspectableFields.Add(inspectableField);
                    isEmpty = false;

                    currentIndex += inspectableField.GetNumLayoutElements();
                }

                base.SetVisible(!isEmpty);
            }
        }

        /// <inheritdoc/>
        protected internal override bool Refresh()
        {
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
            base.SetVisible(!isEmpty && visible);
        }
    }
}
