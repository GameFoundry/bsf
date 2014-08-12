using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableBool : InspectableObjectBase
    {
        private bool oldPropertyValue;
        private GUIToggleField guiField;
        private bool isInitialized;

        public InspectableBool(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Bool)
            {
                guiField = new GUIToggleField(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            bool newPropertyValue = property.GetValue<bool>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            if (guiField != null)
                guiField.Value = property.GetValue<bool>();
        }

        private void OnFieldValueChanged(bool newValue)
        {
            property.SetValue(newValue);
        }
    }
}
