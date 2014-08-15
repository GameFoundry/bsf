using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableVector3 : InspectableObjectBase
    {
        private Vector3 propertyValue;
        private GUIVector3Field guiField;
        private bool isInitialized;

        public InspectableVector3(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Vector3)
            {
                guiField = new GUIVector3Field(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            Vector3 newPropertyValue = property.GetValue<Vector3>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            // TODO - Skip update if it currently has input focus so user can modify the value in peace

            propertyValue = property.GetValue<Vector3>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        private void OnFieldValueChanged(Vector3 newValue)
        {
            property.SetValue(newValue);
        }
    }
}
