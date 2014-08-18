using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableFloat : InspectableObjectBase
    {
        private float propertyValue;
        private GUIFloatField guiFloatField;
        private bool isInitialized;

        public InspectableFloat(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Float)
            {
                guiFloatField = new GUIFloatField(new GUIContent(title));
                guiFloatField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiFloatField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            float newPropertyValue = property.GetValue<float>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            propertyValue = property.GetValue<float>();
            if (guiFloatField != null)
            {
                if (guiFloatField.HasInputFocus())
                    return;

                guiFloatField.Value = propertyValue;
            }
        }

        private void OnFieldValueChanged(float newValue)
        {
            property.SetValue(newValue);
        }
    }
}
