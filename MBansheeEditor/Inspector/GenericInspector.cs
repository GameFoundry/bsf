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
        private bool isInitialized;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

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
