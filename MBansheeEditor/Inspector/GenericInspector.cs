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

                foreach (var field in serializableObject.fields)
                {
                    if (!field.Inspectable)
                        continue;

                    if (field.HasCustomInspector)
                        inspectableFields.Add(InspectableObjectBase.CreateCustomInspectable(field.CustomInspectorType, field.Name, field.GetProperty()));
                    else
                        inspectableFields.Add(InspectableObjectBase.CreateDefaultInspectable(field.Name, field.GetProperty()));
                }
            }


            // TODO - Add Component foldout

            isInitialized = true;
        }

        internal override void Refresh()
        {
            if (!isInitialized)
                Initialize();

            foreach (var field in inspectableFields)
            {
                field.Refresh(GUI.layout);
            }
        }

        internal override int GetOptimalHeight()
        {
            throw new NotImplementedException();
        }
    }
}
