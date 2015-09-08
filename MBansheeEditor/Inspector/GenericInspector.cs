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
        private List<InspectableObjectBase> inspectableFields = new List<InspectableObjectBase>();

        private void Initialize()
        {
            if (referencedObject != null)
            {
                SerializableObject serializableObject = new SerializableObject(referencedObject.GetType(), referencedObject);

                foreach (var field in serializableObject.Fields)
                {
                    if (!field.Inspectable)
                        continue;

                    if (field.HasCustomInspector)
                        inspectableFields.Add(InspectableObjectBase.CreateCustomInspectable(field.CustomInspectorType, field.Name, new InspectableFieldLayout(layout), field.GetProperty()));
                    else
                        inspectableFields.Add(InspectableObjectBase.CreateDefaultInspectable(field.Name, new InspectableFieldLayout(layout), field.GetProperty()));
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
